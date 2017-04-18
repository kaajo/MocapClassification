#include "methodtester.h"

MethodTester::MethodTester()
{

}

void MethodTester::testMethod(const QVector<MocapAnimation*> &anims, const QVector<FunctionProp> &functions)
{
    QVector<Result> results,resultsTmp;
    QVector<float> error;
    cv::Mat distMatrix(anims.size(),anims.size(),CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));

    if (!std::get<2>(functions[0]).empty())
    {
        distMatrix = std::get<2>(functions[0]);
    }

    //1st function (source is anims)
    for (int i = 0; i < anims.size(); ++i)
    {
        auto res = MocapAnimation::getDistance(anims,i,std::get<1>(functions[0]),distMatrix);

        Result r(anims[i],res);
        results.push_back(r);

        r.printResult(anims[i],r);
    }

    for (const int noRes : std::get<3>(functions[0]))
    {
        error.push_back(checkResultsError(results,noRes));
    }

    printMethodError(std::get<0>(functions[0]),std::get<3>(functions[0]),error);
    error.clear();

    for (int f = 1; f < functions.size(); ++f)
    {
        distMatrix = cv::Mat(anims.size(),anims.size(),CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));

        if (!std::get<2>(functions[f]).empty())
        {
            distMatrix = std::get<2>(functions[f]);
        }

        for (int i = 0; i < anims.size(); ++i)
        {
            auto res = MocapAnimation::getDistance(results[i].m_distance,std::get<0>(functions[f]),i,anims[i], std::get<1>(functions[f]),distMatrix);

            Result r(anims[i],res);

            resultsTmp.push_back(r);
            r.printResult(anims[i],r);
        }
        results = resultsTmp;
        resultsTmp.clear();

        for (const int noRes : std::get<3>(functions[f]))
        {
            error.push_back(checkResultsError(results,noRes));
        }

        printMethodError(std::get<0>(functions[f]),std::get<3>(functions[f]),error);
        error.clear();
    }
}

float MethodTester::testMethod(const QVector<MocapAnimation *> &anims, const QVector<WeightedProp> &functions)
{
    cv::Mat mergeMat = std::get<0>(functions[0]) * std::get<1>(functions[0]);
    for (int f = 1; f < functions.size(); ++f)
    {
        if (std::get<0>(functions[f]) == 0.0f) continue;

        cv::multiply(mergeMat,std::get<1>(functions[f]),mergeMat,std::get<0>(functions[f]));
    }

    QVector<Result> results;

    for (int i = 0; i < anims.size(); ++i)
    {
        QVector<QPair<float,MocapAnimation*>> res;

        for (int j = 0; j < anims.size(); ++j)
        {
            if (i == j) continue;

            res.push_back({mergeMat.at<float>(i,j),anims[j]});
        }

        Result r(anims[i],res);

        results.push_back(r);
    }

    float error = checkResultsError(results,{1});

    printMethodError(anims.size(),{1},{error});

    return error;
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
