/*
    Copyright (C) 2017  Miroslav Krajíček

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QVector3D>

#include "helperfunctions.h"
#include "comparefunctions.h"

#include <cassert>
#include <vector>

#include <mocapanimation.h>



#define NUM_OF_NODES 31
/*
namespace SimilarityFunctions {

float movementAmount(const MocapAnimation &first,const MocapAnimation &second, CompareHist::CompareHistogram method)
{
    const auto maq1 = first.getAxisMovementQuantity();
    const auto maq2 = second.getAxisMovementQuantity();

    cv::Mat pnts1, pnts2;
    pnts1.create(1, maq1.size()*3,CV_32FC1);
    pnts2.create(1, maq2.size()*3,CV_32FC1);

    const int s = maq1.size();

    for (int i = 0; i < s; ++i)
    {
        pnts1.at<float>(i) = maq1[i][0];
        pnts2.at<float>(i) = maq2[i][0];
        pnts1.at<float>(s+i) = maq1[i][1];
        pnts2.at<float>(s+i) = maq2[i][1];
        pnts1.at<float>(2*s+i) = maq1[i][2];
        pnts2.at<float>(2*s+i) = maq2[i][2];
    }

    return static_cast<float>(CompareHist::compareHistogramFunction(pnts1,pnts2,method));
}

float movementAmountV1V2(const MocapAnimation &first,const MocapAnimation &second, CompareHist::CompareHistogram method)
{
    const auto maq1 = first.getAxisMovementQuantity();
    const auto maq2 = second.getAxisMovementQuantity();
    const auto maqr1 = first.getMovementQuantityPoints();
    const auto maqr2 = second.getMovementQuantityPoints();

    cv::Mat pnts1, pnts2;
    pnts1.create(1, maq1.size()*4,CV_32FC1);
    pnts2.create(1, maq2.size()*4,CV_32FC1);

    const int s = maq1.size();

    for (int i = 0; i < s; ++i)
    {
        pnts1.at<float>(i) = maq1[i][0];
        pnts2.at<float>(i) = maq2[i][0];
        pnts1.at<float>(s+i) = maq1[i][1];
        pnts2.at<float>(s+i) = maq2[i][1];
        pnts1.at<float>(2*s+i) = maq1[i][2];
        pnts2.at<float>(2*s+i) = maq2[i][2];
        pnts1.at<float>(3*s+i) = maqr1[i];
        pnts2.at<float>(3*s+i) = maqr2[i];
    }

    return static_cast<float>(CompareHist::compareHistogramFunction(pnts1,pnts2,method));
}

float movementAmountV1V2V3Length(const MocapAnimation &first,const MocapAnimation &second, CompareHist::CompareHistogram method)
{
    const auto maq1 = first.getAxisMovementQuantity();
    const auto maq2 = second.getAxisMovementQuantity();
    const auto maqr1 = first.getMovementQuantityPoints();
    const auto maqr2 = second.getMovementQuantityPoints();
    const auto maqar1 = first.getMovementQuantityAxisReduced();
    const auto maqar2 = second.getMovementQuantityAxisReduced();


    cv::Mat pnts1, pnts2;
    pnts1.create(1, maq1.size()*4+4,CV_32FC1);
    pnts2.create(1, maq2.size()*4+4,CV_32FC1);

    const int s = maq1.size();

    for (int i = 0; i < s; ++i)
    {
        pnts1.at<float>(i) = maq1[i][0];
        pnts2.at<float>(i) = maq2[i][0];
        pnts1.at<float>(s+i) = maq1[i][1];
        pnts2.at<float>(s+i) = maq2[i][1];
        pnts1.at<float>(2*s+i) = maq1[i][2];
        pnts2.at<float>(2*s+i) = maq2[i][2];
        pnts1.at<float>(3*s+i) = maqr1[i];
        pnts2.at<float>(3*s+i) = maqr2[i];
    }

    pnts1.at<float>(4*s) = maqar1[0];
    pnts1.at<float>(4*s+1) = maqar1[1];
    pnts1.at<float>(4*s+2) = maqar1[2];
    pnts1.at<float>(4*s+3) = first.frames();

    pnts2.at<float>(4*s) = maqar2[0];
    pnts2.at<float>(4*s+1) = maqar2[1];
    pnts2.at<float>(4*s+2) = maqar2[2];
    pnts2.at<float>(4*s+3) = second.frames();

    return static_cast<float>(CompareHist::compareHistogramFunction(pnts1,pnts2,method));
}


float movementAmountAxisReduced(const MocapAnimation &first,const MocapAnimation &second, CompareHist::CompareHistogram method)
{
    const auto metrics1 = first.getMovementQuantityPoints();
    const auto metrics2 = second.getMovementQuantityPoints();

    cv::Mat m1(1,31,CV_32FC1),m2(1,31,CV_32FC1);

    for (int n = 0; n < 31; ++n)
    {
        m1.at<float>(n) = metrics1[n];
        m2.at<float>(n) = metrics2[n];
    }

    return static_cast<float>(CompareHist::compareHistogramFunction(m1,m2,method));
}

float movementAmountPointsReduced(const MocapAnimation &first,const MocapAnimation &second, CompareHist::CompareHistogram method)
{
    return 0.0f;
}

float movementAmountTotal(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getMovementQuantityPoints();
    auto metrics2 = second.getMovementQuantityPoints();

    float sum1 = std::accumulate(metrics1.begin(), metrics1.end(), 0.0f);
    float sum2 = std::accumulate(metrics2.begin(), metrics2.end(), 0.0f);

    return std::fabs(sum2 - sum1);
}

//////////////////////////////////////////////////////////////////////////////////////////////

float pointMovementDominant(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0f;

    auto metrics1 = first.getMovementQuantityPoints();
    auto metrics2 = second.getMovementQuantityPoints();

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

    auto metrics1 = first.getMovementQuantityPoints();
    auto metrics2 = second.getMovementQuantityPoints();

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

float pointMovementDirectionHistogram(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getAxisMovementDirectionHist();
    auto metrics2 = second.getAxisMovementDirectionHist();

    cv::Mat pnts1, pnts2;

    pnts1.create(1, metrics1.size(),CV_32FC3);
    pnts2.create(1, metrics2.size(),CV_32FC3);

    for (size_t i = 0; i < metrics1.size(); ++i)
    {
        pnts1.at<cv::Vec3f>(i) = metrics1[i];
        pnts2.at<cv::Vec3f>(i) = metrics2[i];
    }

    std::vector<cv::Mat> hist1,hist2;
    cv::split(pnts1,hist1);
    cv::split(pnts2,hist2);


    auto method = CV_COMP_CHISQR;

    double retVal = std::min(cv::compareHist(hist1[0],hist2[0],method) +
                             cv::compareHist(hist1[1],hist2[1],method) +
                             cv::compareHist(hist1[2],hist2[2],method) ,
                             cv::compareHist(hist2[0],hist1[0],method) +
                             cv::compareHist(hist2[1],hist1[1],method) +
                             cv::compareHist(hist2[2],hist1[2],method));

    return std::fabs(retVal);
}

float compareAllAxesMovementQuantity2(const MocapAnimation &first,const MocapAnimation &second)
{
    const auto &maq1 = first.getAxisMovementQuantity();
    const auto &maq2 = second.getAxisMovementQuantity();

    cv::Mat pnts1, pnts2;
    pnts1.create(maq1.size(),3,CV_32FC1);
    pnts2.create(maq2.size(),3,CV_32FC1);

    const size_t s = maq1.size();

    for (size_t i = 0; i < s; ++i)
    {
        pnts1.at<float>(i,0) = maq1[i][0];
        pnts2.at<float>(i,0) = maq2[i][0];
        pnts1.at<float>(i,1) = maq1[i][1];
        pnts2.at<float>(i,1) = maq2[i][1];
        pnts1.at<float>(i,2) = maq1[i][2];
        pnts2.at<float>(i,2) = maq2[i][2];
    }

    const double retVal = std::min(cv::EMD(pnts1,pnts2,CV_DIST_L2) , cv::EMD(pnts1,pnts2,CV_DIST_L2));

    return std::fabs(retVal);
}


float compareAllAxesMovementAcceleration(const MocapAnimation &first,const MocapAnimation &second)
{
    const auto &maq1 = first.getAxisMovementAcc();
    const auto &maq2 = second.getAxisMovementAcc();

    cv::Mat pnts1, pnts2;
    pnts1.create(1, maq1.size()*3,CV_32FC1);
    pnts2.create(1, maq2.size()*3,CV_32FC1);

    const size_t s = maq1.size();

    for (size_t i = 0; i < s; ++i)
    {
        pnts1.at<float>(i) = maq1[i][0];
        pnts2.at<float>(i) = maq2[i][0];
        pnts1.at<float>(s+i) = maq1[i][1];
        pnts2.at<float>(s+i) = maq2[i][1];
        pnts1.at<float>(2*s+i) = maq1[i][2];
        pnts2.at<float>(2*s+i) = maq2[i][2];
    }

    const double retVal = std::min(cv::compareHist(pnts1,pnts2,CV_COMP_CHISQR) , cv::compareHist(pnts2,pnts1,CV_COMP_CHISQR));

    return std::fabs(retVal);
}

float compareAllAxesQuantityAcc(const MocapAnimation &first,const MocapAnimation &second)
{
    const auto &maq1 = first.getAxisMovementQuantity();
    const auto &maq2 = second.getAxisMovementQuantity();
    const auto &macc1 = first.getAxisMovementAcc();
    const auto &macc2 = second.getAxisMovementAcc();

    cv::Mat pnts1, pnts2;
    pnts1.create(1, maq1.size()*6,CV_32FC1);
    pnts2.create(1, maq2.size()*6,CV_32FC1);

    const size_t s = maq1.size();

    for (size_t i = 0; i < s; ++i)
    {
        pnts1.at<float>(i) = maq1[i][0];
        pnts2.at<float>(i) = maq2[i][0];
        pnts1.at<float>(s+i) = maq1[i][1];
        pnts2.at<float>(s+i) = maq2[i][1];
        pnts1.at<float>(2*s+i) = maq1[i][2];
        pnts2.at<float>(2*s+i) = maq2[i][2];
        pnts1.at<float>(3*s+i) = macc1[i][0];
        pnts2.at<float>(3*s+i) = macc2[i][0];
        pnts1.at<float>(4*s+i) = macc1[i][1];
        pnts2.at<float>(4*s+i) = macc2[i][1];
        pnts1.at<float>(5*s+i) = macc1[i][2];
        pnts2.at<float>(5*s+i) = macc2[i][2];
    }

    const double retVal = std::min(cv::compareHist(pnts1,pnts2,CV_COMP_CHISQR) , cv::compareHist(pnts2,pnts1,CV_COMP_CHISQR));

    return std::fabs(retVal);
}

float compareHistFeatures(const MocapAnimation &first,const MocapAnimation &second)
{
    const auto& metrics1 = first.getMovementQuantityPoints();
    const auto& metrics2 = second.getMovementQuantityPoints();

    assert(metrics1.size() == metrics2.size());

    cv::Mat pnts1, pnts2;

    pnts1.create(1, metrics1.size()*2+4,CV_32FC1);
    pnts2.create(1, metrics2.size()*2+4,CV_32FC1);

    for (size_t i = 0; i < metrics1.size(); ++i)
    {
        pnts1.at<float>(i) = metrics1[i];
        pnts2.at<float>(i) = metrics2[i];
    }
    for (size_t i = 0; i < metrics1.size(); ++i)
    {
        pnts1.at<float>(i + metrics1.size()) = cv::norm(first.getAxisMovementAcc()[i]);
        pnts2.at<float>(i + metrics1.size()) = cv::norm(second.getAxisMovementAcc()[i]);
    }

    const auto &maq1 = first.getAxisMovementQuantity();
    cv::Vec3f sumAxis1 = std::accumulate(maq1.begin(),maq1.end(),cv::Vec3f(0.0f,0.0f,0.0f));

    pnts1.at<float>(metrics1.size()*2) = sumAxis1[0];
    pnts1.at<float>(metrics1.size()*2+1) = sumAxis1[1];
    pnts1.at<float>(metrics1.size()*2+2) = sumAxis1[2];
    pnts1.at<float>(metrics1.size()*2+3) = first.frames();

    const auto &maq2 = second.getAxisMovementQuantity();
    cv::Vec3f sumAxis2 = std::accumulate(maq2.begin(),maq2.end(),cv::Vec3f(0.0f,0.0f,0.0f));

    pnts2.at<float>(metrics2.size()*2) = sumAxis2[0];
    pnts2.at<float>(metrics2.size()*2+1) = sumAxis2[1];
    pnts2.at<float>(metrics2.size()*2+2) = sumAxis2[2];
    pnts2.at<float>(metrics2.size()*2+3) = second.frames();

    const double retVal = std::min(cv::compareHist(pnts1,pnts2,CV_COMP_CHISQR) , cv::compareHist(pnts2,pnts1,CV_COMP_CHISQR));

    return std::fabs(retVal);
}

float pointMovementCorrelation2(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getMovementQuantityPoints();
    auto metrics2 = second.getMovementQuantityPoints();
    auto movDir1 = first.getAxisMovementDirectionHist();
    auto movDir2 = second.getAxisMovementDirectionHist();

    cv::Mat pnts1, pnts2;

    pnts1.create(1, metrics1.size()*2+movDir1.size()*3+4,CV_32FC1);
    pnts2.create(1, metrics2.size()*2+movDir1.size()*3+4,CV_32FC1);

    for (size_t i = 0; i < metrics1.size(); ++i)
    {
        pnts1.at<float>(i) = metrics1[i];
        pnts2.at<float>(i) = metrics2[i];
    }
    for (size_t i = 0; i < metrics1.size(); ++i)
    {
        pnts1.at<float>(i + metrics1.size()) = cv::norm(first.getAxisMovementAcc()[i]);
        pnts2.at<float>(i + metrics1.size()) = cv::norm(second.getAxisMovementAcc()[i]);
    }
    for (size_t i = 0; i < movDir1.size(); ++i)
    {
        pnts1.at<float>(i+metrics1.size()*2) = movDir1[i][0];
        pnts1.at<float>(i+metrics1.size()*2+movDir1.size()) = movDir1[i][1];
        pnts1.at<float>(i+metrics1.size()*2+movDir1.size()*2) = movDir1[i][2];
        pnts2.at<float>(i+metrics1.size()*2) = movDir2[i][0];
        pnts2.at<float>(i+metrics1.size()*2+movDir1.size()) = movDir2[i][1];
        pnts2.at<float>(i+metrics1.size()*2+movDir1.size()*2) = movDir2[i][2];
    }

    auto maq1 = first.getAxisMovementQuantity();
    cv::Vec3f sumAxis1 = std::accumulate(maq1.begin(),maq1.end(),cv::Vec3f(0.0f,0.0f,0.0f));

    pnts1.at<float>(metrics1.size()*2+movDir1.size()*3) = sumAxis1[0];
    pnts1.at<float>(metrics1.size()*2+movDir1.size()*3+1) = sumAxis1[1];
    pnts1.at<float>(metrics1.size()*2+movDir1.size()*3+2) = sumAxis1[2];
    pnts1.at<float>(metrics1.size()*2+movDir1.size()*3+3) = first.frames();

    auto maq2 = second.getAxisMovementQuantity();
    cv::Vec3f sumAxis2 = std::accumulate(maq2.begin(),maq2.end(),cv::Vec3f(0.0f,0.0f,0.0f));

    pnts2.at<float>(metrics2.size()*2+movDir1.size()*3) = sumAxis2[0];
    pnts2.at<float>(metrics2.size()*2+movDir1.size()*3+1) = sumAxis2[1];
    pnts2.at<float>(metrics2.size()*2+movDir1.size()*3+2) = sumAxis2[2];
    pnts2.at<float>(metrics2.size()*2+movDir1.size()*3+3) = second.frames();

    double retVal = std::min(cv::compareHist(pnts1,pnts2,CV_COMP_CHISQR) , cv::compareHist(pnts2,pnts1,CV_COMP_CHISQR));

    return std::fabs(retVal);
}

float totalMovementRelativeError(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getMovementQuantityPoints();
    auto metrics2 = second.getMovementQuantityPoints();;

    float sum1 = std::accumulate(metrics1.begin(), metrics1.end(), 0.0f);
    float sum2 = std::accumulate(metrics2.begin(), metrics2.end(), 0.0f);

    return std::fabs((sum2 - sum1)/std::fmax(sum1,sum2));
}

float dominantPoints(const MocapAnimation &first,const MocapAnimation &second)
{
    auto metrics1 = first.getMovementQuantityPoints();
    auto metrics2 = second.getMovementQuantityPoints();;

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
    *
}

float pointDistance(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0;

    const int size = std::min(first.frames(),second.frames());

    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < 31; ++j)
        {
            error += cv::norm(first(j,i) - second(j,i));
        }
    }

    assert(error >= 0.0f);

    return error;
}

float pointDistanceNorm(const MocapAnimation &first,const MocapAnimation &second)
{
    float error = 0.0;

    const int size = std::min(first.frames(),second.frames());

    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < 31; ++j)
        {
            error += cv::norm(first(j,i) - second(j,i));
        }
    }

    assert(error >= 0.0f);

    return error/static_cast<float>(size);
}


float pointDistanceAlligned(const MocapAnimation &first,const MocapAnimation &second)
{
    float minError = std::numeric_limits<float>::max();

    int size = (first.frames() > second.frames()) ? first.frames() : second.frames();

    for (int a = -20; a < 20; ++a)
    {
        float error = 0.0;

        for(int i = 0; i < size; ++i)
        {
            for(int j = 0; j < 31; ++j)
            {
                error += cv::norm(first(j,i+a) - second(j,i));
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
    QVector<QVector<float>> mGamma(first.frames(), QVector<float>(second.frames(),1000000.0));
    mGamma[0][0] = 0.0;

    const int secSize = std::max(second.frames()/4, std::abs(first.frames()-second.frames()));

    for( int i = 1; i < first.frames() -1; ++i )
    {
        const int maxj = std::min(second.frames() - 1, i+secSize);

        for( int j = std::max(1, i - secSize); j < maxj; ++j )
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                cost += cv::norm(first(k,i) - second(k,j));
                cost += std::pow(cv::norm(first(k,i+1)-first(k,i-1)) -
                                 cv::norm(second(k,j+1)-second(k,j-1)),2);
            }

            mGamma[i][j] = cost + std::min({mGamma[i-1][j], mGamma[i][j-1], mGamma[i-1][j-1]});
        }
    }

    return mGamma[first.frames() - 1][second.frames() - 1];
}

float MDDTWNorm(const MocapAnimation &first,const MocapAnimation &second)
{
    QVector<QVector<float>> mGamma(first.frames(), QVector<float>(second.frames(),1000000.0));
    mGamma[0][0] = 0.0;

    const int secSize = std::max(second.frames()/4, std::abs(first.frames()-second.frames()));

    for( int i = 1; i < first.frames() -1; ++i )
    {
        const int maxj = std::min(second.frames() - 1, i+secSize);

        for( int j = std::max(1, i - secSize); j < maxj; ++j )
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                cost += cv::norm(first(k,i) - second(k,j));
                cost += std::pow(cv::norm(first(k,i+1)-first(k,i-1)) -
                                 cv::norm(second(k,j+1)-second(k,j-1)),2);
            }

            mGamma[i][j] = cost + std::min({mGamma[i-1][j], mGamma[i][j-1], mGamma[i-1][j-1]});
        }
    }

    return mGamma[first.frames() - 1][second.frames() - 1] / std::max(first.frames(),second.frames());
}


float MDDDTW(const MocapAnimation &first,const MocapAnimation &second)
{
    QVector<QVector<float>> mGamma(first.frames(), QVector<float>(second.frames(),1000000.0));
    mGamma[0][0] = 0.0;

    const int secSize = std::max(second.frames()/4, std::abs(first.frames()-second.frames()));

    for( int i = 1; i < first.frames() -1; ++i )
    {
        const int maxj = std::min(second.frames() - 1, i+secSize);

        for( int j = std::max(1, i - secSize); j < maxj; ++j )
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                cost += cv::norm(first(k,i) - second(k,j));
                cost += std::pow(cv::norm(first(k,i+1)-first(k,i-1)) -
                                 cv::norm(second(k,j+1)-second(k,j-1)),2);
            }

            mGamma[i][j] = cost + std::min({mGamma[i-1][j], mGamma[i][j-1], mGamma[i-1][j-1]});
        }
    }

    return mGamma[first.frames() - 2][second.frames() - 2];
}

float MDDDTWNorm(const MocapAnimation &first,const MocapAnimation &second)
{
    cv::Mat mGamma(first.frames(), second.frames(), CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));
    mGamma.at<float>(0,0) = 0.0f;

    const int secSize = std::max(second.frames()/4, std::abs(first.frames()-second.frames()));

    for(int i = 1; i < first.frames() -1; ++i)
    {
        const int maxj = std::min(second.frames() - 1, i+secSize);

        for(int j = std::max(1, i - secSize); j < maxj; ++j)
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                cost += cv::norm(first(k,i) - second(k,j));
                cost += std::pow(cv::norm(first(k,i+1)-first(k,i-1)) -
                                 cv::norm(second(k,j+1)-second(k,j-1)),2);
            }

            mGamma.at<float>(i,j) = cost + std::min({mGamma.at<float>(i-1,j), mGamma.at<float>(i,j-1), mGamma.at<float>(i-1,j-1)});
        }
    }

    return mGamma.at<float>(first.frames() - 2,second.frames() - 2)/std::max(first.frames(),second.frames());
}

float MDCentralDTWNorm(const MocapAnimation &first,const MocapAnimation &second)
{
    cv::Mat mGamma(first.frames(), second.frames(), CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));
    mGamma.at<float>(0,0) = 0.0f;

    const int secSize = std::max(second.frames()/4, std::abs(first.frames()-second.frames()));

    for(int i = 1; i < first.frames() -2; ++i)
    {
        const int maxj = std::min(second.frames() - 2, i+secSize);

        for(int j = std::max(1, i - secSize); j < maxj; ++j)
        {
            float cost = 0.0f;

            for (int k = 0; k < NUM_OF_NODES; ++k)
            {
                const float &fDiff = (cv::norm(first(k,i) - first(k,i-1)) + cv::norm(first(k,i+1) - first(k,i-1))/2.0f)/2.0f;
                const float &sDiff = (cv::norm(second(k,j) - second(k,j-1)) + cv::norm(second(k,j+1) - second(k,j-1))/2.0f)/2.0f;

                cost += std::abs(fDiff - sDiff);
            }

            mGamma.at<float>(i,j) = cost + std::min({mGamma.at<float>(i-1,j), mGamma.at<float>(i,j-1), mGamma.at<float>(i-1,j-1)});
        }
    }

    return mGamma.at<float>(first.frames() - 2,second.frames() - 2)/std::max(first.frames(),second.frames());
}

float fourierDescriptors(const MocapAnimation &first,const MocapAnimation &second)
{
    auto fdFirst = first.getAxisFourierDescriptor();
    auto fdSecond = second.getAxisFourierDescriptor();

    float error = 0.0f;

    for (size_t i = 0; i < fdFirst.size(); ++i)
    {
        for (int j = 0; j < fdFirst[0].rows; ++j)
        {
            error += cv::norm(fdFirst[i].row(j),fdSecond[i].row(j),cv::NORM_L2SQR);
        }
    }

    return error;
}

float pointCorrelationVariance(const MocapAnimation &first,const MocapAnimation &second)
{
    for (size_t i = 0; i < NUM_OF_NODES; ++i)
    {
        auto fCol = first(i);
        auto sCol = second(i);

        if (fCol.cols > sCol.cols)
        {
            int dif = fCol.cols - sCol.cols;
            fCol = fCol(cv::Rect(0,dif/2,1,sCol.cols));
        }
        else
        {
            int dif = sCol.cols - fCol.cols;
            sCol = sCol(cv::Rect(0,dif/2,1,fCol.cols));
        }

        std::vector<cv::Mat> f,s;
        cv::split(fCol,f);
        cv::split(sCol,s);

        for (size_t j = 0; j < f.size(); ++j)
        {
            cv::Mat fb,sb;

            cv::copyMakeBorder(f[i],fb,1,1,1,1,cv::BORDER_CONSTANT,cv::Scalar(0.0f));
            cv::copyMakeBorder(s[i],sb,1,1,1,1,cv::BORDER_CONSTANT,cv::Scalar(0.0f));

            cv::Point bestMatch = cv::phaseCorrelate(fb,sb);

            std::cout << bestMatch << std::endl;
        }
    }

    return 0.0f;
}

float MSEVoxels(const MocapAnimation &first,const MocapAnimation &second)
{
    return static_cast<float>(first.getVoxelMap().MSE(second.getVoxelMap()));
}

float DICECoefficientVoxels(const MocapAnimation &first,const MocapAnimation &second)
{
    cimg_library::CImg<int> f = first.getVoxelMap();
    cimg_library::CImg<int> s = second.getVoxelMap();

    cimg_library::CImg<int> tp = f.get_min(s);
    cimg_library::CImg<int> fp = f - tp;
    cimg_library::CImg<int> fn = s - tp;

    auto tpHist = tp.get_histogram(2,0,1);
    auto fpHist = fp.get_histogram(2,0,1);
    auto fnHist = fn.get_histogram(2,0,1);

    float dice = 2.0f*static_cast<float>(tpHist(1))/static_cast<float>((2*tpHist(1) + fpHist(1) + fnHist(1)));

    if(dice < 0.001f)
    {
        return std::numeric_limits<float>::max();
    }

    return 1.0f/dice;
}

float DistanceTransformVoxels(const MocapAnimation &first,const MocapAnimation &second)
{
    cimg_library::CImg<int> f = first.getVoxelMap();
    cimg_library::CImg<int> s = second.getVoxelMap();

    float error = 0.0;

    //TODO future work

    return error;
}

} //SimilarityFunctions
*/
#endif // FUNCTIONS_H
