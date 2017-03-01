#include "methodtester.h"

MethodTester::MethodTester()
{

}

void MethodTester::TestMethod(const QVector<MocapAnimation*> &anims, const QVector<FunctionProp> &functions)
{
    QVector<Result> results,resultsTmp;
    QVector<float> error;
    QVector<QVector<float>> distMatrix(anims.size(),QVector<float>(anims.size(),std::numeric_limits<float>::max()));

    //1st function (source is anims)
    for (int i = 0; i < anims.size(); ++i)
    {
        auto res = MocapAnimation::getDistance(anims,i,std::get<1>(functions[0]),distMatrix);

        Result r(anims[i],res);
        results.push_back(r);

        r.printResult(anims[i],r);
    }

    for (const int noRes : std::get<2>(functions[0]))
    {
        error.push_back(checkResultsError(results,noRes));
    }

    printMethodError(std::get<0>(functions[0]),std::get<2>(functions[0]),error);
    error.clear();

    for (int f = 1; f < functions.size(); ++f)
    {
        distMatrix = QVector<QVector<float>>(anims.size(),QVector<float>(anims.size(),std::numeric_limits<float>::max()));

        for (int i = 0; i < anims.size(); ++i)
        {
            auto res = MocapAnimation::getDistance(results[i].m_distance,std::get<0>(functions[f]),i,anims[i], std::get<1>(functions[f]),distMatrix);

            Result r(anims[i],res);

            resultsTmp.push_back(r);
            r.printResult(anims[i],r);
        }
        results = resultsTmp;
        resultsTmp.clear();

        for (const int noRes : std::get<2>(functions[f]))
        {
            error.push_back(checkResultsError(results,noRes));
        }

        printMethodError(std::get<0>(functions[f]),std::get<2>(functions[f]),error);
        error.clear();
    }
}

float MethodTester::checkResultsError(const QVector<Result> &prevResults, const int numOfResults)
{
    int trueCounter = 0;

    for (int i = 0; i < prevResults.size(); ++i)
    {
        for (auto j = 0; j < numOfResults && j < prevResults[i].m_distance.size(); ++j)
        {
            if (prevResults[i].m_distance[j].second->getRealCategory() ==
                    prevResults[i].m_animiation->getRealCategory())
            {
                ++trueCounter;
                break;
            }
        }
    }

    return trueCounter/(float)prevResults.size();
}

void MethodTester::printMethodError(int numOfPrevResults, QVector<int> checkSet, QVector<float> output)
{
    std::cout << "used " << numOfPrevResults << " from previous method" << std::endl;
    for(int i = 0; i < output.size(); ++i)
    {
        std::cout << checkSet[i] << ": " << output[i] * 100.0f << " %" << std::endl;
    }
    std::cout << "________________" << std::endl;
}
