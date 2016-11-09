#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <mocapanimation.h>

#include <vector>

#include <QVector3D>

#define NUM_OF_NODES 31

namespace SimilarityFunctions {

float dirVec(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0;

    int size = (first.size() > second.size()) ? first.size() : second.size();

    for(int i = 1; i < size; ++i)
    {
        for(int j = 0; j < first[i].size(); ++j)
        {
            if(!first[i-1].empty() && !second[i-1].empty())
            {
                QVector3D fdir(first[i][j] - first[i-1][j]);
                QVector3D sdir(second[i][j] - second[i-1][j]);

                error += std::abs(std::atan2(QVector3D::crossProduct(fdir, sdir).length(), QVector3D::dotProduct(fdir, sdir)));
            }
        }
    }

    return error;
}

float pointDistanceDerivative(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0;

    int size = (first.size() > second.size()) ? first.size() : second.size();

    std::vector<float> prevDist(31,0.0);

    for(int i = 0; i < size; ++i)
    {
        //assert(first[i].size() == second[i].size());

        std::vector<float> curDist(31,0.0);

        for(int j = 0; j < first[i].size(); ++j)
        {
            curDist[j] = (first[i][j] - second[i][j]).length();

            error += std::abs(curDist[j] - prevDist[j]);
        }

        prevDist = curDist;
    }

    return error;
}

float pointDist(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0;

    int size = (first.size() > second.size()) ? first.size() : second.size();

    std::vector<float> prevDist(31,0.0);

    for(int i = 0; i < size; ++i)
    {

        std::vector<float> curDist(31,0.0);

        float move = 1.0;
        float currError = 0.0;

        for(int j = 0; j < first[i].size(); ++j)
        {
            curDist[j] = (first[i][j] - second[i][j]).length();

            //move += std::abs((first[i][j] - first[i][j-1]).length());

            currError += std::abs(curDist[j] - prevDist[j]);
        }

        prevDist = curDist;
        error += currError/move;
    }

    return error /= size;
}

float pointDistance(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0;

    int size = (first.size() > second.size()) ? first.size() : second.size();

    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < first[i].size(); ++j)
        {
            error += (first[i][j] - second[i][j]).length();
        }
    }

    return error;
}

}

namespace MetricFunctions {

float nodeMovementQuantity(const MocapAnimation &anim, int nodeNum)
{
    if(nodeNum >= NUM_OF_NODES || nodeNum < 0)
    {
        return 0.0;
    }

    float retVal = 0.0f;

    for (int i = 1; i < anim.size(); ++i)
    {
        retVal += (anim[i][nodeNum] - anim[i-1][nodeNum]).length();
    }

    return retVal;
}

}


#endif // FUNCTIONS_H
