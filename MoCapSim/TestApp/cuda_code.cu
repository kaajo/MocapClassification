////////////////////////////////////////////////////////////////////////////
//
// Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
//
// Please refer to the NVIDIA end user license agreement (EULA) associated
// with this source code for terms and conditions that govern your use of
// this software. Any use, reproduction, disclosure, or distribution of
// this software and related documentation outside the terms of the EULA
// is strictly prohibited.
//
////////////////////////////////////////////////////////////////////////////

//
// This sample illustrates the usage of CUDA events for both GPU timing and
// overlapping CPU and GPU execution.  Events are inserted into a stream
// of CUDA calls.  Since CUDA stream calls are asynchronous, the CPU can
// perform computations while GPU is executing (including DMA memcopies
// between the host and device).  CPU can query CUDA events to determine
// whether GPU has completed tasks.
//

// includes, system
#include <stdio.h>

// includes CUDA Runtime
#include <cuda.h>
#include <cuda_runtime.h>

// includes, project
#include <helper_cuda.h>
#include <helper_functions.h> // helper utility functions
#include <helper_math.h>

#define MARKERS  31
#define BLOCK_SIZE 32

template<class T>
struct SharedMemory
{
    __device__ inline operator       T *()
    {
        extern __shared__ int __smem[];
        return (T *)__smem;
    }

    __device__ inline operator const T *() const
    {
        extern __shared__ int __smem[];
        return (T *)__smem;
    }
};

__global__ void pointDistance_kernel(float3 *first, float3 *second, float *retVal, int size)
{
    unsigned int tid = threadIdx.x;
    int idx =  blockIdx.x * blockDim.x * blockDim.y + threadIdx.y * blockDim.x + threadIdx.x;

    if(idx >= size) return;

    float res =  length(first[idx] - second[idx]);

    retVal[idx] = res;

    __syncthreads();

    unsigned int i = blockIdx.x*BLOCK_SIZE + threadIdx.x;
    unsigned int gridSize = BLOCK_SIZE*gridDim.x;

    float *sdata = SharedMemory<float>();

    float mySum = 0.0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < size)
    {
        mySum += retVal[i];

        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (i + BLOCK_SIZE < size)
            mySum += retVal[i+BLOCK_SIZE];

        i += gridSize;
    }

    // each thread puts its local sum into shared memory
    sdata[tid] = mySum;

    __syncthreads();

    if ((BLOCK_SIZE >=  32) && (tid < 16))
    {
        sdata[tid] = mySum = mySum + sdata[tid + 16];
    }

    __syncthreads();

    if ((BLOCK_SIZE >=  16) && (tid <  8))
    {
        sdata[tid] = mySum = mySum + sdata[tid +  8];
    }

    __syncthreads();

    if ((BLOCK_SIZE >=   8) && (tid <  4))
    {
        sdata[tid] = mySum = mySum + sdata[tid +  4];
    }

    __syncthreads();

    if ((BLOCK_SIZE >=   4) && (tid <  2))
    {
        sdata[tid] = mySum = mySum + sdata[tid +  2];
    }

    __syncthreads();

    if ((BLOCK_SIZE >=   2) && ( tid <  1))
    {
        sdata[tid] = mySum = mySum + sdata[tid +  1];
    }

    __syncthreads();

    // write result for this block to global mem
    if (tid == 0) retVal[blockIdx.x] = mySum;
}

bool correct_output(int *data, const int n, const int x)
{
    for (int i = 0; i < n; i++)
        if (data[i] != x)
        {
            printf("Error! data[%d] = %d, ref = %d\n", i, data[i], x);
            return false;
        }

    return true;
}

template <class T, unsigned int blockSize, bool nIsPow2>
__global__ void reduce6(T *g_idata, T *g_odata, unsigned int n)
{
    T *sdata = SharedMemory<T>();

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockSize*2 + threadIdx.x;
    unsigned int gridSize = blockSize*2*gridDim.x;

    T mySum = 0;

    // we reduce multiple elements per thread.  The number is determined by the
    // number of active thread blocks (via gridDim).  More blocks will result
    // in a larger gridSize and therefore fewer elements per thread
    while (i < n)
    {
        mySum += g_idata[i];

        // ensure we don't read out of bounds -- this is optimized away for powerOf2 sized arrays
        if (nIsPow2 || i + blockSize < n)
            mySum += g_idata[i+blockSize];

        i += gridSize;
    }

    // each thread puts its local sum into shared memory
    sdata[tid] = mySum;
    __syncthreads();


    // do reduction in shared mem
    if ((blockSize >= 512) && (tid < 256))
    {
        sdata[tid] = mySum = mySum + sdata[tid + 256];
    }

    __syncthreads();

    if ((blockSize >= 256) &&(tid < 128))
    {
        sdata[tid] = mySum = mySum + sdata[tid + 128];
    }

    __syncthreads();

    if ((blockSize >= 128) && (tid <  64))
    {
        sdata[tid] = mySum = mySum + sdata[tid +  64];
    }

    __syncthreads();

    if ( tid < 32 )
    {
        // Fetch final intermediate sum from 2nd warp
        if (blockSize >=  64) mySum += sdata[tid + 32];
        // Reduce final warp using shuffle
        for (int offset = warpSize/2; offset > 0; offset /= 2)
        {
            mySum += __shfl_down(mySum, offset);
        }
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[blockIdx.x] = mySum;
}

extern "C"
int cuda_main(int argc, char *argv[], std::vector<std::vector<std::vector<float3>>> anims)
{
    int devID;
    cudaDeviceProp deviceProps;

    printf("[%s] - Starting...\n", argv[0]);

    // This will pick the best possible CUDA capable device
    devID = findCudaDevice(argc, (const char **)argv);

    // get device name
    checkCudaErrors(cudaGetDeviceProperties(&deviceProps, devID));
    printf("CUDA device [%s]\n", deviceProps.name);

    std::vector<std::vector<float3>> first = anims[0];
    std::vector<std::vector<float3>> second = anims[1];

    int firstSize = MARKERS * first.size();
    int firstBytes = firstSize * sizeof(float3);

    int secondSize = MARKERS * second.size();
    int secondBytes = secondSize * sizeof(float3);

    float3 *f = &(first[0][0]);
    float3 *s = &(second[0][0]);

    float3 *d_f;
    float3 *d_s;

    // allocate memory
    checkCudaErrors(cudaMalloc((void**) &d_f ,firstBytes));
    checkCudaErrors(cudaMalloc((void**) &d_s,secondBytes));
    cudaMemcpy(d_f, f, firstBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_s, s, secondBytes, cudaMemcpyHostToDevice);

    // set kernel launch configuration
    int size = (first.size() < second.size()) ? first.size() : second.size();
    int resultSize = size * MARKERS ;

    dim3 threads(BLOCK_SIZE, MARKERS);
    dim3 blocks(size/BLOCK_SIZE + 1,1 /*first.size()/4*/);

    printf("executing %d blocks result size %d\n", size/BLOCK_SIZE + 1, resultSize);

    // create cuda event handles
    cudaEvent_t start, stop;
    checkCudaErrors(cudaEventCreate(&start));
    checkCudaErrors(cudaEventCreate(&stop));

    StopWatchInterface *timer = NULL;
    sdkCreateTimer(&timer);
    sdkResetTimer(&timer);

    checkCudaErrors(cudaDeviceSynchronize());
    float gpu_time = 0.0f;

    float * results;
    checkCudaErrors(cudaMallocManaged((void**) &results,resultSize * sizeof(float)));


    // asynchronously issue work to the GPU (all to stream 0)
    sdkStartTimer(&timer);
    cudaEventRecord(start, 0);
    pointDistance_kernel<<<blocks, threads>>>(d_f, d_s,results, resultSize);
    cudaEventRecord(stop, 0);
    cudaDeviceSynchronize();
    sdkStopTimer(&timer);

    // have CPU do some work while waiting for stage 1 to finish
    unsigned long int counter=0;

    while (cudaEventQuery(stop) == cudaErrorNotReady)
    {
        counter++;
    }

    checkCudaErrors(cudaEventElapsedTime(&gpu_time, start, stop));

    // print the cpu and gpu times
    printf("time spent executing by the GPU: %.2f\n", gpu_time);
    printf("time spent by CPU in CUDA calls: %.2f\n", sdkGetTimerValue(&timer));
    printf("CPU executed %lu iterations while waiting for GPU to finish\n", counter);

    float result = results[0];
    /*for (size_t i = 0; i < resultSize; ++i)
    {
        //printf(" %f ", results[i]);
        result += results[i];
    }*/
    result /=  first.size();

    printf("\n");
    printf("GPU result for size %d: %f\n", size, result);

    // release resources
    checkCudaErrors(cudaEventDestroy(start));
    checkCudaErrors(cudaEventDestroy(stop));
    checkCudaErrors(cudaFree(d_f));
    checkCudaErrors(cudaFree(d_s));
    checkCudaErrors(cudaFree(results));

    // cudaDeviceReset causes the driver to clean up all state. While
    // not mandatory in normal operation, it is good practice.  It is also
    // needed to ensure correct operation when the application is being
    // profiled. Calling cudaDeviceReset causes all profile data to be
    // flushed before the application exits
    cudaDeviceReset();

    return EXIT_SUCCESS;
}

