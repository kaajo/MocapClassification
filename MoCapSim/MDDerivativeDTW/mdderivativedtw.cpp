/*
    Copyright (C) 2019  Miroslav Krajíček

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

#include "mdderivativedtw.h"

#include <QtConcurrent/QtConcurrentMap>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>

#include <iostream>

MDDerivativeDTW::MDDerivativeDTW(QObject *parent) : QObject(parent)
{
    connect(&m_descWatcher,&QFutureWatcher<cimg_library::CImg<float>>::finished,this, &MDDerivativeDTW::onComputeDescFinished);
    connect(&m_distanceWatcher,&QFutureWatcher<void>::finished,this, &MDDerivativeDTW::onComputeDistAllFinished);
}

void MDDerivativeDTW::setAnimations(QVector<MocapAnimation *> animations)
{
    m_anims = animations;
    m_distanceMatrix = cv::Mat(m_anims.size(),m_anims.size(),CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));
}

QFuture<void> MDDerivativeDTW::computeDescriptors()
{

    const QFuture<QPair<int,cv::Mat>> future = QtConcurrent::mapped(m_anims,&MDDerivativeDTW::computeGradient);
    m_descWatcher.setFuture(future);
    return future;
}

QFuture<void> MDDerivativeDTW::computeAllDistances()
{
    std::function<void(const MocapAnimation*)> f = std::bind(&MDDerivativeDTW::computeDistToAll,std::placeholders::_1, m_distanceMatrix, m_anims, m_descriptors);
    const QFuture<void> future = QtConcurrent::map(m_anims, f);
    m_distanceWatcher.setFuture(future);
    return future;
}

QWidget *MDDerivativeDTW::getVisualization()
{
    return nullptr;
}

void MDDerivativeDTW::selectionAdded(int animId)
{
    refreshVis();
}

void MDDerivativeDTW::selectionRemoved(int animId)
{
    refreshVis();
}

void MDDerivativeDTW::onComputeDescFinished()
{
    const auto future = m_descWatcher.future();

    for (auto it = future.constBegin(); it != future.constEnd(); ++it)
    {
        m_descriptors[it->first] = it->second;
    }
}

void MDDerivativeDTW::onComputeDistAllFinished()
{

}

float MDDerivativeDTW::computeDist(const MocapAnimation &first, const MocapAnimation &second)
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

            for (int k = 0; k < 31; ++k)
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

float MDDerivativeDTW::computeDistOpt(const MocapAnimation &first, const MocapAnimation &second, const cv::Mat &descFirst, const cv::Mat &descSecond)
{
    cv::Mat mGamma(first.frames(), second.frames(), CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));
    mGamma.at<float>(0,0) = 0.0f;

    const int secSize = std::max(second.frames()/4, std::abs(first.frames()-second.frames()));

    for(int fFrame = 1; fFrame < first.frames() -1; ++fFrame)
    {
        const int maxj = std::min(second.frames() - 1, fFrame+secSize);

        float *pfFrameRow = mGamma.ptr<float>(fFrame);
        float *pfFrameRowMinus = mGamma.ptr<float>(fFrame-1);

        for(int sFrame = std::max(1, fFrame - secSize); sFrame < maxj; ++sFrame)
        {
            float cost = 0.0f;

            for (int k = 0; k < 31; ++k)
            {
                cost += cv::norm(first(k,fFrame) - second(k,sFrame));
                cost += (descFirst.at<float>(fFrame,k) - descSecond.at<float>(sFrame,k)) *
                        (descFirst.at<float>(fFrame,k) - descSecond.at<float>(sFrame,k));
            }

            pfFrameRow[sFrame] = cost + std::min({pfFrameRowMinus[sFrame], pfFrameRow[sFrame-1], pfFrameRowMinus[sFrame-1]});
        }
    }

    return mGamma.at<float>(first.frames() - 2,second.frames() - 2)/std::max(first.frames(),second.frames());
}

QPair<int,cv::Mat> MDDerivativeDTW::computeGradient(const MocapAnimation * const anim)
{
    const cv::Mat posesInTime = anim->getPosesInTime();
    cv::Mat gradient(posesInTime.cols -1,posesInTime.rows, CV_32FC1);

    for (int row = 0; row < posesInTime.rows; ++row)
    {
        for (int col = 1; col < posesInTime.cols - 1; ++col)
        {
            gradient.at<float>(col,row) = cv::norm((*anim)(row,col+1),(*anim)(row,col-1));
        }
    }

    return {anim->getId(),gradient};
}

void MDDerivativeDTW::computeDistToAll(const MocapAnimation *anim, cv::Mat &reducedResults, QVector<MocapAnimation *> anims, QHash<int,cv::Mat> descriptors)
{
    const int id = anim->getId();
    auto it = anims.begin();

    while (id > (*it)->getId())
    {
        const int id2 = (*it)->getId();
/*
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        const float dist = computeDist(**it,*anim);
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

        std::chrono::high_resolution_clock::time_point start2 = std::chrono::high_resolution_clock::now();
        const float distNew = computeDistOpt(**it,*anim,descriptors[(**it).getId()],descriptors[(*anim).getId()]);
        std::chrono::high_resolution_clock::time_point end2 = std::chrono::high_resolution_clock::now();

        qDebug() << "old:" << std::chrono::duration_cast<std::chrono::duration<double>>(end-start).count()
                 << "new:" << std::chrono::duration_cast<std::chrono::duration<double>>(end2-start2).count();

        if (std::abs(dist - distNew) > 0.001) {
            qDebug() << "BAD: " << dist << " new: " << distNew;
        }
*/

        reducedResults.at<float>(id,id2) =
        reducedResults.at<float>(id2,id) = computeDistOpt(**it,*anim,descriptors[(**it).getId()],descriptors[(*anim).getId()]);
        ++it;
    }
}

void MDDerivativeDTW::refreshVis()
{

}
