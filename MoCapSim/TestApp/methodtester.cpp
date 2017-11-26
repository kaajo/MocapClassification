#include <resultvisualization.hpp>

#include "methodtester.h"

#include <mocapanimation.h>

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

        ResultMetrics metr(results);

        for (const int noRes : std::get<3>(functions[f]))
        {
            const float err = metr.getAccuracy(noRes);

            error.push_back(err);
        }

        printMethodError(std::get<0>(functions[f]),std::get<3>(functions[f]),error);
        printMethodCatAccuracy(metr.getCategoryAccuracy(1));
        printAverageFirstMatch(results);
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

    ResultMetrics metr(results);

    float error = metr.getAccuracy(1);

    printMethodError(anims.size(),{1},{error});

    return error;
}

void MethodTester::printMethodCatAccuracy(QMap<int16_t, qreal> catAcc)
{
    qreal avg = 0.0;

    std::cout << "accuracy in categories: " << std::endl;
    for (int16_t k : catAcc.keys())
    {
        avg += catAcc[k];
        std::cout << k << " " << catAcc[k] << std::endl;
    }

    avg /= static_cast<qreal>(catAcc.size());

    qreal standDev = 0.0;

    for (int16_t k : catAcc.keys())
    {
        standDev +=  std::pow(catAcc[k] - avg,2);
    }

    standDev = sqrt(1.0/(static_cast<qreal>(catAcc.size()) - 1) * standDev);

    std::cout << "stand. dev. :" << standDev << std::endl;
}

void MethodTester::printMethodError(int numOfPrevResults, QVector<int> checkSet, QVector<float> output)
{
    std::cout << "used " << numOfPrevResults << " from previous method" << std::endl;
    for(int i = 0; i < output.size(); ++i)
    {
        std::cout << checkSet[i] << " " << output[i] * 100.0f << " %" << std::endl;
    }
    std::cout << "________________" << std::endl;
}

void MethodTester::printAverageFirstMatch(QVector<Result> &results)
{
    int sum = 0;

    for (int i = 0; i < results.size(); ++i)
    {
        int matched = results[i].firstMatched();
        if (matched > 0)
        {
            sum += matched;
        }
    }

    std::cout << "avg first match: "
              << static_cast<float>(sum)/static_cast<float>(results.size())
              << std::endl;
}


