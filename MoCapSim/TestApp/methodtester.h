#ifndef METHODTESTER_H
#define METHODTESTER_H

#include "result.h"

#include <QVector>

#include <ostream>

//first is number of results from previous method, similarity function and set of "top N" results to print
typedef std::tuple<int,MocapAnimation::SimilarityFunction,QVector<int>> FunctionProp;

class MethodTester
{
public:
    MethodTester();

    static void TestMethod(const QVector<MocapAnimation*> &anims,const QVector<FunctionProp> &functions);

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
