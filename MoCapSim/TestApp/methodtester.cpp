#include "methodtester.h"

QVector<int> MethodTester::m_skipCategories  = {56, 57,58,59,60, 61 , 138 , 139};

MethodTester::MethodTester()
{

}

void MethodTester::testMethod(const QVector<MocapAnimation*> &anims, const QVector<FunctionProp> &functions)
{
    QVector<Result> results,resultsTmp;
    QVector<float> error;
    cv::Mat distMatrix;

    for (int i = 0; i < anims.size(); ++i)
    {
        QVector<QPair<float,MocapAnimation*>> res;

        for (int j = 0; j < anims.size(); ++j)
        {
            if (j == i) continue;

            res.push_back({0.0f,anims[j]});
        }

        Result r(anims[i],res);
        results.push_back(r);
    }

    for (int f = 0; f < functions.size(); ++f)
    {
        distMatrix = cv::Mat(anims.size(),anims.size(),CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));

        if (!std::get<2>(functions[f]).empty())
        {
            distMatrix = std::get<2>(functions[f]);
        }

        for (int i = 0; i < anims.size(); ++i)
        {
            auto res = anims[i]->getDistance(results[i].distance(),std::get<0>(functions[f]), std::get<1>(functions[f]),distMatrix);

            Result r(anims[i],res);

            resultsTmp.push_back(r);
            r.printResult();
        }
        results = resultsTmp;
        resultsTmp.clear();

        for (const int noRes : std::get<3>(functions[f]))
        {
            const float err = checkResultsError(results,noRes);

            ResultMetrics metr(results);
            float error2 = metr.getAccuracy();
            auto error3 = metr.getCategoryAccuracy();

            error.push_back(err);
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
    int animsCounter = 0;

    for (int i = 0; i < prevResults.size(); ++i)
    {
        if (!m_skipCategories.contains(prevResults[i].animation()->getRealCategory()))
        {
            ++animsCounter;
        }
        else
        {
            std::cout << "skip anim ID: " << prevResults[i].animation()->getId() << std::endl;
            continue;
        }

        if (prevResults[i].isCategoryMatched(numOfResults))
        {
            ++trueCounter;
        }
    }

    return trueCounter/(float)animsCounter;
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

void MethodTester::setSkipCategories(const QVector<int> &skipCategories)
{
    m_skipCategories = skipCategories;
}
