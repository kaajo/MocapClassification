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

#include <QtConcurrent/QtConcurrentMap>


#include <opencv2/highgui.hpp>

#include <QTime>

#include "dicecoefficientvoxels.h"

DICECoefficientVoxels::DICECoefficientVoxels(QObject *parent) : QObject(parent)
{
    connect(&m_descWatcher,&QFutureWatcher<cimg_library::CImg<float>>::progressValueChanged,this, &DICECoefficientVoxels::onComputeDesc);
    connect(&m_distanceWatcher,&QFutureWatcher<void>::progressValueChanged,this, &DICECoefficientVoxels::onComputeDesc);
    connect(&m_descWatcher,&QFutureWatcher<cimg_library::CImg<float>>::finished,this, &DICECoefficientVoxels::onComputeDescFinished);
    connect(&m_distanceWatcher,&QFutureWatcher<void>::finished,this, &DICECoefficientVoxels::onComputeDistAllFinished);
}

void DICECoefficientVoxels::setAnimations(QVector<MocapAnimation *> animations)
{
    m_anims = animations;
    m_distanceMatrix = cv::Mat(m_anims.size(),m_anims.size(),CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));
}

QFuture<void> DICECoefficientVoxels::computeDescriptors()
{
    const QFuture<QPair<int,cimg_library::CImg<uint8_t>>> future = QtConcurrent::mapped(m_anims,&DICECoefficientVoxels::computeVoxels);
    m_descWatcher.setFuture(future);
    return future;
}

QFuture<void> DICECoefficientVoxels::computeAllDistances()
{
    std::function<void(const MocapAnimation*)> f = std::bind(&DICECoefficientVoxels::computeDistToAll,std::placeholders::_1, m_distanceMatrix, m_anims);
    const QFuture<void> future = QtConcurrent::map(m_anims, f);
    m_distanceWatcher.setFuture(future);
    return future;
}

float DICECoefficientVoxels::getDistance(MocapAnimation &anim1, MocapAnimation &anim2)
{
    if (m_distanceMatrix.at<float>(anim1.getId(),anim2.getId()) < std::numeric_limits<float>::max())
    {
        return m_distanceMatrix.at<float>(anim1.getId(),anim2.getId());
    }
    else
    {
        const float res = computeDist(anim1,anim2);
        m_distanceMatrix.at<float>(anim1.getId(),anim2.getId()) =
        m_distanceMatrix.at<float>(anim2.getId(),anim1.getId()) = res;

        return res;
    }
}

QWidget* DICECoefficientVoxels::getVisualization()
{
    return m_vis;
}

void DICECoefficientVoxels::selectionAdded(int animId)
{
    m_visImgs.insert(animId,m_descriptors[animId]);
    refreshVis();
}

void DICECoefficientVoxels::selectionRemoved(int animId)
{
    m_visImgs.remove(animId);
    refreshVis();
}

void DICECoefficientVoxels::onComputeDesc(int progress)
{
    qDebug() << "compute descriptors progress:" << progress;
}

void DICECoefficientVoxels::onComputeDescFinished()
{
    const auto future = m_descWatcher.future();

    for (auto it = future.constBegin(); it != future.constEnd(); ++it)
    {
        m_descriptors[it->first] = it->second;

    }
}

void DICECoefficientVoxels::onComputeDistAllFinished()
{
    cv::imwrite("/home/mkrajicek/Documents/SDIPR/mocap-segmenting/DICEVoxelsHDM65-alt.exr",m_distanceMatrix);
}

float DICECoefficientVoxels::computeDist(const MocapAnimation &first, const MocapAnimation &second)
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

void DICECoefficientVoxels::computeDistToAll(const MocapAnimation *anim,cv::Mat &reducedResults,QVector<MocapAnimation*> anims)
{
    const int id = anim->getId();
    auto it = anims.begin();

    while (id > (*it)->getId())
    {
        const int id2 = (*it)->getId();
        reducedResults.at<float>(id,id2) =
        reducedResults.at<float>(id2,id) = computeDist(**it,*anim);
        ++it;
    }
}

QPair<int,cimg_library::CImg<uint8_t>> DICECoefficientVoxels::computeVoxels(const MocapAnimation * const anim)
{
    int color = 1;
    int m_numberOfNodes = 31;

    cimg_library::CImg<float> m_voxelMap = cimg_library::CImg<float>(20,20,20,1,0);

    for (int n = 0; n < m_numberOfNodes; ++n)
    {
        for (int f = 1; f < anim->frames(); ++f)
        {
            const cv::Vec3f p = (*anim)(n,f);
            const cv::Vec3f &pos = ((*anim)(n,f) + cv::Vec3f(20.0f,20.0f,20.0f)) * 5.0f/10.0f;
            const cv::Vec3f &prevPos = ((*anim)(n,f-1) + cv::Vec3f(20.0f,20.0f,20.0f)) * 5.0f/10.0f;

            const cv::Vec<uint,3> &posi = cv::Vec3i(std::nearbyintf(pos[0]),std::nearbyintf(pos[1]),std::nearbyintf(pos[2]));
            const cv::Vec<uint,3> &prevPosi = cv::Vec3i(std::nearbyintf(prevPos[0]),std::nearbyintf(prevPos[1]),std::nearbyintf(prevPos[2]));

            if (prevPosi == posi)
            {
                m_voxelMap(posi[0],posi[1],posi[2]) = 1.0f;
            }
            else
            {
                m_voxelMap.draw_line(prevPosi[0],prevPosi[1],prevPosi[2],posi[0],posi[1],posi[2],&color);
            }
        }
    }

    return {anim->getId(),m_voxelMap};
}

void DICECoefficientVoxels::refreshVis()
{
    m_vis->update(m_visImgs);
}
