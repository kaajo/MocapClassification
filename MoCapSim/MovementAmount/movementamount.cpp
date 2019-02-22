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
#include "movementamount.h"

#include <QtConcurrent/QtConcurrentMap>
#include <QTime>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

MovementAmount::MovementAmount(QObject *parent) : QObject(parent)
{
    connect(&m_descWatcher,&QFutureWatcher<cimg_library::CImg<float>>::finished,this, &MovementAmount::onComputeDescFinished);
    connect(&m_distanceWatcher,&QFutureWatcher<void>::finished,this, &MovementAmount::onComputeDistAllFinished);
}

void MovementAmount::setAnimations(QVector<MocapAnimation *> animations)
{
    m_anims = animations;
    m_distanceMatrix = cv::Mat(m_anims.size(),m_anims.size(),CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));
}

QFuture<void> MovementAmount::computeDescriptors()
{
    const QFuture<QPair<int,std::array<cv::Vec3f,31>>> future = QtConcurrent::mapped(m_anims,&MovementAmount::computeDescriptor);
    m_descWatcher.setFuture(future);
    return future;
}

QFuture<void> MovementAmount::computeAllDistances()
{
    std::function<void(const MocapAnimation*)> f = std::bind(&MovementAmount::computeDistToAll,std::placeholders::_1, m_distanceMatrix, m_anims, m_descriptors);
    const QFuture<void> future = QtConcurrent::map(m_anims, f);
    m_distanceWatcher.setFuture(future);
    return future;
}

float MovementAmount::getDistance(MocapAnimation &anim1, MocapAnimation &anim2)
{
    if (m_distanceMatrix.at<float>(anim1.getId(),anim2.getId()) < std::numeric_limits<float>::max())
    {
        return m_distanceMatrix.at<float>(anim1.getId(),anim2.getId());
    }
    else
    {
        const float res = computeDist(m_descriptors[anim1.getId()], m_descriptors[anim2.getId()]);
        m_distanceMatrix.at<float>(anim1.getId(),anim2.getId()) =
        m_distanceMatrix.at<float>(anim2.getId(),anim1.getId()) = res;

        return res;
    }
}

QWidget *MovementAmount::getVisualization()
{
    return m_vis;
}

void MovementAmount::selectionAdded(int animId)
{
    m_selectedDescriptors.insert(animId,m_descriptors[animId]);
    refreshVis();
}

void MovementAmount::selectionRemoved(int animId)
{
    m_selectedDescriptors.remove(animId);
    refreshVis();
}

void MovementAmount::onComputeDescFinished()
{
    const auto future = m_descWatcher.future();

    for (auto it = future.constBegin(); it != future.constEnd(); ++it)
    {
        m_descriptors[it->first] = it->second;

    }
}

void MovementAmount::onComputeDistAllFinished()
{
    cv::imwrite("/home/mkrajicek/Documents/SDIPR/mocap-segmenting/MovementAmountHDM65-alt.exr",m_distanceMatrix);
}

void normHistogram(cv::Mat &hist)
{
    float sum1 = 0.0f;

    for (int i = 0; i < hist.cols; ++i)
    {
        sum1 += hist.at<float>(i);
    }

    if ((sum1 - 1.0f) < 0.01f) return;

    for (int i = 0; i < hist.cols; ++i)
    {
        hist.at<float>(i) = hist.at<float>(i)/sum1;
    }
}

float MovementAmount::computeDist(std::array<cv::Vec3f,31> desc1, std::array<cv::Vec3f,31> desc2)
{
    cv::Mat pnts1, pnts2;
    pnts1.create(1, desc1.size()*3,CV_32FC1);
    pnts2.create(1, desc2.size()*3,CV_32FC1);

    const int s = desc1.size();

    for (int i = 0; i < s; ++i)
    {
        pnts1.at<float>(i) = desc1[i][0];
        pnts2.at<float>(i) = desc2[i][0];
        pnts1.at<float>(s+i) = desc1[i][1];
        pnts2.at<float>(s+i) = desc2[i][1];
        pnts1.at<float>(2*s+i) = desc1[i][2];
        pnts2.at<float>(2*s+i) = desc2[i][2];
    }

    normHistogram(pnts1);
    normHistogram(pnts2);

    return static_cast<float>(std::min(cv::compareHist(pnts1,pnts2,CV_COMP_CHISQR),
                                       cv::compareHist(pnts2,pnts1,CV_COMP_CHISQR)));
}

void MovementAmount::computeDistToAll(const MocapAnimation *anim, cv::Mat &reducedResults, QVector<MocapAnimation *> anims,
                                      QHash<int, std::array<cv::Vec3f,31>> descriptors)
{
    const int id = anim->getId();
    auto it = anims.begin();

    while (id > (*it)->getId())
    {
        const int id2 = (*it)->getId();
        reducedResults.at<float>(id,id2) =
        reducedResults.at<float>(id2,id) = computeDist(descriptors[(*it)->getId()],descriptors[anim->getId()]);
        ++it;
    }
}

QPair<int,std::array<cv::Vec3f,31>> MovementAmount::computeDescriptor(const MocapAnimation * const anim)
{
    std::array<cv::Vec3f,31> descriptor;

    for (size_t n = 0; n < 31; ++n)
    {
        cv::Vec3f q = 0.0f;

        for (int i = 1; i < anim->frames(); ++i)
        {
            cv::Vec3f diff;
            cv::absdiff((*anim)(n,i), (*anim)(n,i-1),diff);
            q += diff;
        }

        descriptor[n] = q;
    }

    return {anim->getId(),descriptor};
}

void MovementAmount::refreshVis()
{
    m_vis->update(m_selectedDescriptors);
}
