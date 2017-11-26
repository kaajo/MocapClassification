#ifndef METHODTESTER_H
#define METHODTESTER_H

#include "result.h"
#include "resultstats.hpp"

#include <opencv2/core/core.hpp>

#include <QVector>

#include <ostream>

class MocapAnimation;

// int - number of results from previous method, similarity function, set of "top N" results to print
typedef std::tuple<int,std::function<float(const MocapAnimation&,const MocapAnimation&)>,cv::Mat,QVector<int>> FunctionProp;
// float - weight of method
typedef std::tuple<float,cv::Mat> WeightedProp;

class MethodTester
{
public:
    MethodTester();

    static void testMethod(const QVector<MocapAnimation*> &anims,const QVector<FunctionProp> &functions);
    static float testMethod(const QVector<MocapAnimation*> &anims, const QVector<WeightedProp> &functions);

    static void printMethodCatAccuracy(QMap<int16_t, qreal> catAcc);
    static void printMethodError(int numOfPrevResults, QVector<int> checkSet, QVector<float> output);
    static void printAverageFirstMatch(QVector<Result> &results);
};

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr)
{
    std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));
    return o;
}

#endif // METHODTESTER_H
