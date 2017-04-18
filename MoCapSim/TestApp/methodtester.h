#ifndef METHODTESTER_H
#define METHODTESTER_H

#include "result.h"

#include <opencv2/core/core.hpp>

#include <QVector>

#include <ostream>

// int - number of results from previous method, similarity function, set of "top N" results to print
typedef std::tuple<int,MocapAnimation::SimilarityFunction,cv::Mat,QVector<int>> FunctionProp;
typedef std::tuple<float,cv::Mat> WeightedProp;

class MethodTester
{
public:
    MethodTester();

    static void testMethod(const QVector<MocapAnimation*> &anims,const QVector<FunctionProp> &functions);
    static float testMethod(const QVector<MocapAnimation*> &anims, const QVector<WeightedProp> &functions);

    static float checkResultsError(const QVector<Result> &prevResults, const int numOfResults);

    static void printMethodError(int numOfPrevResults, QVector<int> checkSet, QVector<float> output);
};

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr)
{
    std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));
    return o;
}

#endif // METHODTESTER_H
