#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <mocapanimation.h>

#include <QVector3D>
#include <QDebug>

#include <opencv2/opencv.hpp>

#include <cassert>
#include <vector>

#define NUM_OF_NODES 31

namespace SimilarityFunctions {

float pointMovementAbsoluteError(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0f;

    auto metrics1 = first.getMovementQuantity();
    auto metrics2 = second.getMovementQuantity();

    for (size_t n = 0; n < 31; ++n)
    {
        if (metrics1[n] == 0.0f || metrics2[n] == 0.0f) continue;

        float e = std::fabs((metrics2[n] - metrics1[n]));

        error += std::pow(e,2);
    }

    assert(error >= 0.0f);

    return sqrtf(error);
}

float pointMovementDominant(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0f;

    auto metrics1 = first.getMovementQuantity();
    auto metrics2 = second.getMovementQuantity();

    float sum1 = std::accumulate(metrics1.begin(), metrics1.end(), 0.0f);
    float sum2 = std::accumulate(metrics2.begin(), metrics2.end(), 0.0f);

    for (size_t n = 1; n < 31; ++n)
    {
        if (metrics1[n] == 0.0f || metrics2[n] == 0.0f) continue;
        if (metrics1[n]/sum1 < 0.01f && metrics2[n]/sum2 < 0.01f) continue;

        float e = std::fabs((metrics2[n] - metrics1[n]));

        error += std::pow(e,2);
    }

    assert(error >= 0.0f);

    return sqrtf(error);
}

float pointMovementRelativeError(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0f;

    auto metrics1 = first.getMovementQuantity();
    auto metrics2 = second.getMovementQuantity();;

    float sum1 = std::accumulate(metrics1.begin(), metrics1.end(), 0.0f);
    float sum2 = std::accumulate(metrics2.begin(), metrics2.end(), 0.0f);

    for (size_t n = 0; n < NUM_OF_NODES; ++n)
    {
        if (metrics1[n] == 0.0f || metrics2[n] == 0.0f) continue;

        float e = std::fabs((metrics2[n]/sum2 - metrics1[n]/sum1) / std::fmax(metrics2[n]/sum2,metrics1[n]/sum1));

        error += std::pow(e,2);
    }

    assert(error >= 0.0f);

    return sqrtf(error);
}

float totalMovementRelativeError(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getMovementQuantity();
    auto metrics2 = second.getMovementQuantity();;

    float sum1 = std::accumulate(metrics1.begin(), metrics1.end(), 0.0f);
    float sum2 = std::accumulate(metrics2.begin(), metrics2.end(), 0.0f);

    return std::fabs((sum2 - sum1)/std::fmax(sum1,sum2));
}

float totalMovementAbsoluteError(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getMovementQuantity();
    auto metrics2 = second.getMovementQuantity();

    float sum1 = std::accumulate(metrics1.begin(), metrics1.end(), 0.0f);
    float sum2 = std::accumulate(metrics2.begin(), metrics2.end(), 0.0f);

    return std::fabs(sum2 - sum1);
}

float dominantPoints(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getMovementQuantity();
    auto metrics2 = second.getMovementQuantity();;

    float sum1 = std::accumulate(metrics1.begin(), metrics1.end(), 0.0f);
    float sum2 = std::accumulate(metrics2.begin(), metrics2.end(), 0.0f);

    QMap<float,int> ordered1, ordered2;

    for (int n = 0; n < NUM_OF_NODES; ++n)
    {
        ordered1[metrics1[n]/sum1] = n;
        ordered2[metrics2[n]/sum2] = n;
    }

    int intersectCounter = 0;

    for (int i = 0; i < 10; ++i)
    {
        auto nthPt = (ordered1.end() - i - 1);

        for (int j = 0; j < 10; ++j)
        {
            auto nthPt2 = (ordered2.end() - i -1);

            if (nthPt.value() == nthPt2.value())
            {
                ++intersectCounter;
                break;
            }
        }
    }

    return (NUM_OF_NODES - intersectCounter)/100000.0f;

    /*
    if (intersectCounter > 5)
    {
        return 0.0f;
    }


    return 100000.0f;
    */
}

float pointDistance(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0;

    int size = (first.size() < second.size()) ? first.size() : second.size();

    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < 31; ++j)
        {
            error += (first[i][j] - second[i][j]).length();
        }
    }

    assert(error >= 0.0f);

    return error/(float) size;
}


float pointDistanceAlligned(const MocapAnimation &first,const MocapAnimation &second)
{
    float minError = std::numeric_limits<float>::max();

    int size = (first.size() > second.size()) ? first.size() : second.size();

    for (int a = -20; a < 20; ++a)
    {
        float error = 0.0;

        for(int i = 0; i < size; ++i)
        {
            for(int j = 0; j < 31; ++j)
            {
                error += (first[i+a][j] - second[i][j]).length();
            }
        }

        if (error < minError)
        {
            minError = error;
        }
    }

    assert(minError >= 0.0f);

    return minError/(float) size;
}

float MDDTW(const MocapAnimation &first,const MocapAnimation &second)
{
    QVector<QVector<float>> mGamma(first.size(), QVector<float>(second.size(),10000.0));

    mGamma[0][0] = 0.0;

    int secSize =  std::max(second.size()/2, std::abs(first.size()-second.size()));

    for( int i = 1; i < first.size(); i++ )
    {
        for( int j = std::max(1, i - secSize); j < std::min(second.size(), i+secSize); j++ )
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                cost += (first[i][k] - first[j][k]).length();
            }

            mGamma[i][j] = cost + std::min({mGamma[i-1][j], mGamma[i][j-1], mGamma[i-1][j-1]});
        }
    }
    return mGamma[first.size() - 1][second.size() - 1] / std::max(first.size(),second.size());
}

//http://msp.ewi.tudelft.nl/sites/default/files/DTW-vASCI.pdf
float MDDDTW(const MocapAnimation &first,const MocapAnimation &second)
{
    QVector<QVector<float>> mGamma(first.size(), QVector<float>(second.size(),1000000.0));
    mGamma[0][0] = 0.0;

    int secSize =  std::max(second.size()/4, std::abs(first.size()-second.size()));

    for( int i = 1; i < first.size() -1; ++i )
    {
        for( int j = std::max(1, i - secSize); j < std::min(second.size() - 1, i+secSize); ++j )
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                cost += (first[i][k] - second[j][k]).length();
                cost += std::pow((first[i+1][k]-first[i-1][k]).length() - (second[j+1][k] - second[j-1][k]).length(),2);
            }

            mGamma[i][j] = cost + std::min({mGamma[i-1][j], mGamma[i][j-1], mGamma[i-1][j-1]});
        }
    }

    return mGamma[first.size() - 2][second.size() - 2];
}

float MDDDTWNorm(const MocapAnimation &first,const MocapAnimation &second)
{
    QVector<QVector<float>> mGamma(first.size(), QVector<float>(second.size(),1000000.0));
    mGamma[0][0] = 0.0;

    int secSize = std::max(second.size()/4, std::abs(first.size()-second.size()));

    for( int i = 1; i < first.size() -1; ++i )
    {
        for( int j = std::max(1, i - secSize); j < std::min(second.size() - 1, i+secSize); ++j )
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                cost += (first[i][k] - second[j][k]).length();
                cost += std::pow((first[i+1][k]-first[i-1][k]).length() - (second[j+1][k] - second[j-1][k]).length(),2);
            }

            mGamma[i][j] = cost + std::min({mGamma[i-1][j], mGamma[i][j-1], mGamma[i-1][j-1]});
        }
    }

    return mGamma[first.size() - 2][second.size() - 2]/std::max(first.size(),second.size());
}

float discreteVoxels(const MocapAnimation &first,const MocapAnimation &second)
{
    auto firstVoxels = first.getVoxelMap();
    auto secondVoxels = second.getVoxelMap();

    int missCounter = 0;

    for (auto &x : firstVoxels.keys())
    {
        for (auto &y : firstVoxels[x].keys())
        {
            for (auto &z : firstVoxels[x][y].keys())
            {
                missCounter += std::abs(firstVoxels[x][y][z] - secondVoxels[x][y][z]);
            }
        }
    }

    firstVoxels = first.getVoxelMap();
    secondVoxels = second.getVoxelMap();

    for (auto &x : secondVoxels.keys())
    {
        for (auto &y : secondVoxels[x].keys())
        {
            for (auto &z : secondVoxels[x][y].keys())
            {
                missCounter += std::abs(firstVoxels[x][y][z] - secondVoxels[x][y][z]);
            }
        }
    }

    return missCounter/static_cast<float>((first.size() + second.size())*NUM_OF_NODES);
}

} //SimilarityFunctions

#endif // FUNCTIONS_H
