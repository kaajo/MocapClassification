// includes CUDA Runtime
#include <cuda.h>
#include <cuda_runtime.h>

// includes, project
#include <helper_cuda.h>
#include <helper_functions.h> // helper utility functions
#include <helper_math.h>

#define MARKERS  31


__global__ void pointsMovement(float3 *data, float *firstDifference, float *secondDifference, int size)
{
    int idx = blockIdx.x * blockDim.x * blockDim.y + threadIdx.y * blockDim.x + threadIdx.x;

    if (idx > size)
    {
        return;
    }

    firstDifference[idx] = length(data[idx + MARKERS] - data[idx]);

    __syncthreads();

    //secondDifference[idx] = length(firstDifference[idx + MARKERS] - firstDifference[idx]);
}
