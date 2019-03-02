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

#include <QtConcurrentMap>

#include "mocapanimation.h"

#include <chrono>


#include <opencv2/imgproc/imgproc.hpp>

typedef std::chrono::high_resolution_clock hiresclock;

MocapAnimation::MocapAnimation(const int id, const int category, const QVector<MocapFrame> poses) :
    m_id(id),
    m_categoryId(category),
    m_posesInTime(poses[0].size(),poses.size(), CV_32FC3)
{
    for (int i = 0; i < poses.size(); ++i)
    {
        //std::array<BodyNode*,31> treePose;

        for (int j = 0; j < poses[0].size(); ++j)
        {
            const auto &pos = cv::Vec3f(poses[i][j].x(),poses[i][j].y(),poses[i][j].z());
            m_posesInTime.at<cv::Vec3f>(j,i) = pos;
            //treePose[j] = new BodyNode(BodyNode::NODE(j+1),pos);
        }

        //m_treePosesInTime.push_back(treePose);
    }
/*
    createTreeStructure();

    computeTotalMovementQuantity();
    computeMovementQuantityAxisReduced();
    computeMovementQuantityPoints();
    computeAxisMovementQuantity();

    computeAxisMovementDirectionHist();
    computeAxisMovementAcc();
    computeVoxels();
    computeAxisFourierDescriptors();
    computeDFCFourierDescriptors();
    */
}

MocapAnimation::~MocapAnimation()
{
}
/*
QVector<QPair<float, MocapAnimation*> > MocapAnimation::getDistance(const QVector<QPair<float, MocapAnimation*>> &prevResults, const int topn, const SimilarityFunction function, cv::Mat &distanceMat) const
{
    QVector<MocapAnimation*> resultsReduced;
    QVector<QPair<float,MocapAnimation*>> resultsDone;

    for (int i = 0; i < prevResults.size() && i < topn; ++i)
    {
        if (distanceMat.at<float>(prevResults[i].second->m_id,m_id) >= std::numeric_limits<float>::max())
        {
            resultsReduced.push_back(prevResults[i].second);
        }
        else
        {
            resultsDone.push_back({distanceMat.at<float>(prevResults[i].second->m_id,m_id),prevResults[i].second});
        }
    }

    QVector<QPair<float,MocapAnimation*>> tmpRes =
            QtConcurrent::blockingMapped<QVector<QPair<float,MocapAnimation*>>>
            (resultsReduced.begin(),resultsReduced.end(),std::bind(mapFun,std::placeholders::_1,this,function));

    for(int i = 0; i < tmpRes.size(); ++i)
    {
        distanceMat.at<float>(m_id,tmpRes[i].second->m_id) = distanceMat.at<float>(tmpRes[i].second->m_id,m_id) = tmpRes[i].first;
    }

    tmpRes += resultsDone;
    return tmpRes;
}

double MocapAnimation::PPScomputetotalMovementQuantity()
{
    hiresclock::time_point start = hiresclock::now();
    computeTotalMovementQuantity();
    hiresclock::time_point end = hiresclock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end-start);

    return static_cast<double>(m_posesInTime.cols * m_posesInTime.rows)/time_span.count();
}

double MocapAnimation::PPScomputeMovementQuantityAxisReduced()
{
    hiresclock::time_point start = hiresclock::now();
    computeMovementQuantityAxisReduced();
    hiresclock::time_point end = hiresclock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end-start);

    return static_cast<double>(m_posesInTime.cols * m_posesInTime.rows)/time_span.count();
}

double MocapAnimation::PPScomputeAxisMovementQuantity()
{
    hiresclock::time_point start = hiresclock::now();
    computeAxisMovementQuantity();
    hiresclock::time_point end = hiresclock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end-start);

    return static_cast<double>(m_posesInTime.cols * m_posesInTime.rows)/time_span.count();
}

double MocapAnimation::PPScomputeMovementQuantityPoints()
{
    hiresclock::time_point start = hiresclock::now();
    computeMovementQuantityPoints();
    hiresclock::time_point end = hiresclock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end-start);

    return static_cast<double>(m_posesInTime.cols * m_posesInTime.rows)/time_span.count();
}

double MocapAnimation::PPScomputeVoxels()
{
    hiresclock::time_point start = hiresclock::now();
    computeVoxels();
    hiresclock::time_point end = hiresclock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end-start);

    return static_cast<double>(m_posesInTime.cols * m_posesInTime.rows)/time_span.count();
}

QPair<float,MocapAnimation*> MocapAnimation::mapFun(MocapAnimation *it,const MocapAnimation *anim, MocapAnimation::SimilarityFunction function)
{
    return QPair<float,MocapAnimation*>(function(*anim,*it),it);
}

void MocapAnimation::createTreeStructure()
{
    for (size_t i = 0; i < m_treePosesInTime.size(); ++i)
    {
        for (size_t j = 0; j < m_treePosesInTime[0].size(); ++j)
        {
            BodyNode::NODE nodeType = m_treePosesInTime[i][j]->getNodeType();
            BodyNode::NODE parentType = BodyNode::getParentForType(nodeType);
            std::vector<BodyNode::NODE> childrenTypes = BodyNode::getChildrenForType(nodeType);

            if (parentType != BodyNode::NODE::UNDEFINED)
            {
                BodyNode* parent = m_treePosesInTime[i][static_cast<std::underlying_type<BodyNode::NODE>::type>(parentType) - 1];
                m_treePosesInTime[i][j]->setParent(parent);
            }

            for (size_t ch = 0; ch < childrenTypes.size(); ++ch)
            {
                if (childrenTypes[ch] != BodyNode::NODE::UNDEFINED)
                {
                    BodyNode* child = m_treePosesInTime[i][static_cast<std::underlying_type<BodyNode::NODE>::type>(childrenTypes[ch]) - 1];

                    m_treePosesInTime[i][j]->addChild(child);
                }
            }

        }
    }
}

std::vector<std::array<BodyNode*,31>> MocapAnimation::getTreePosesInTime() const
{
    return m_treePosesInTime;
}

void MocapAnimation::computeTotalMovementQuantity()
{
    float q = 0.0;
    for (size_t n = 0; n < 31; ++n)
    {
        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            q += cv::norm(m_posesInTime.at<cv::Vec3f>(n,i) - m_posesInTime.at<cv::Vec3f>(n,i-1));
        }
    }

    m_totalMovementQuantity = q;
}

void MocapAnimation::computeMovementQuantityAxisReduced()
{
    cv::Vec3f q(0.0f,0.0f,0.0f);
    for (size_t n = 0; n < 31; ++n)
    {
        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            cv::Vec3f diff;
            cv::absdiff(m_posesInTime.at<cv::Vec3f>(n,i), m_posesInTime.at<cv::Vec3f>(n,i-1),diff);
            q += diff;
        }
    }

    m_movementQuantityAxisReduced = q;
}

void MocapAnimation::computeMovementQuantityPoints()
{
    for (size_t n = 0; n < 31; ++n)
    {
        float q = 0.0f;

        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            q += cv::norm(m_posesInTime.at<cv::Vec3f>(n,i) - m_posesInTime.at<cv::Vec3f>(n,i-1));
        }

        m_movementQuantityPoints[n] = q;
    }
}

void MocapAnimation::computeAxisMovementQuantity()
{
    for (size_t n = 0; n < 31; ++n)
    {
        cv::Vec3f q = 0.0f;

        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            cv::Vec3f diff;
            cv::absdiff(m_posesInTime.at<cv::Vec3f>(n,i), m_posesInTime.at<cv::Vec3f>(n,i-1),diff);
            q += diff;
        }

        m_axisMovementQuantity[n] = q;
    }
}

void MocapAnimation::computeAxisMovementDirectionHist()
{
    for (size_t n = 0; n < 31; ++n)
    {
        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            cv::Vec3f diff = m_posesInTime.at<cv::Vec3f>(n,i) - m_posesInTime.at<cv::Vec3f>(n,i-1);

            float dirXY = (std::atan2(diff[1],diff[0]) + M_PI)* 180.0/M_PI;
            float dirXZ = (std::atan2(diff[2],diff[0]) + M_PI)* 180.0/M_PI;
            float dirYZ = (std::atan2(diff[2],diff[1]) + M_PI)* 180.0/M_PI;

            ++m_axisMovementDirectionHist[std::nearbyintf(dirXY/5.0f)][0];
            ++m_axisMovementDirectionHist[std::nearbyintf(dirXZ/5.0f)][1];
            ++m_axisMovementDirectionHist[std::nearbyintf(dirYZ/5.0f)][2];
        }
    }
}

void MocapAnimation::computeAxisMovementAcc()
{
    for (size_t n = 0; n < 31; ++n)
    {
        cv::Vec3f q = 0.0f;

        for (int i = 2; i < m_posesInTime.cols; ++i)
        {
            cv::Vec3f diff1;
            cv::absdiff(m_posesInTime.at<cv::Vec3f>(n,i), m_posesInTime.at<cv::Vec3f>(n,i-1),diff1);
            cv::Vec3f diff2;
            cv::absdiff(m_posesInTime.at<cv::Vec3f>(n,i-1), m_posesInTime.at<cv::Vec3f>(n,i-2),diff2);
            cv::Vec3f diff;
            cv::absdiff(diff1,diff2,diff);

            q += diff;
        }

        m_axisMovementAcc[n] = q;
    }
}

void MocapAnimation::computeVoxels()
{
    int color = 1.0f;

    for (int n = 0; n < m_posesInTime.rows; ++n)
    {
        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            const cv::Vec3f p = m_posesInTime.at<cv::Vec3f>(n,i);
            const cv::Vec3f &pos = (m_posesInTime.at<cv::Vec3f>(n,i) + cv::Vec3f(20.0f,20.0f,20.0f)) * 5.0f/10.0f;
            const cv::Vec3f &prevPos = (m_posesInTime.at<cv::Vec3f>(n,i-1) + cv::Vec3f(20.0f,20.0f,20.0f)) * 5.0f/10.0f;

            const cv::Vec3i &posi = cv::Vec3i(std::nearbyintf(pos[0]),std::nearbyintf(pos[1]),std::nearbyintf(pos[2]));
            const cv::Vec3i &prevPosi = cv::Vec3i(std::nearbyintf(prevPos[0]),std::nearbyintf(prevPos[1]),std::nearbyintf(prevPos[2]));

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
}

void MocapAnimation::computeAxisFourierDescriptors()
{
    std::vector<cv::Mat> channels;
    cv::split(m_posesInTime,channels);

    for (size_t i = 0; i < channels.size(); ++i)
    {
        cv::Mat padded;
        int m = cv::getOptimalDFTSize(channels[i].rows);
        int n = cv::getOptimalDFTSize(channels[i].cols); // on the border add zero values
        cv::copyMakeBorder(channels[i], padded, 0, m - channels[i].rows, 0, n - channels[i].cols, cv::BORDER_CONSTANT, cv::Scalar::all(0.0));

        cv::Mat fd;
        cv::dft(padded,fd, cv::DFT_ROWS | cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);

        m_axisfd.push_back(fd);
    }
}

void MocapAnimation::computeDFCFourierDescriptors()
{
    std::vector<cv::Mat> channels;
    cv::split(m_posesInTime,channels);

    for (int i = 0; i < m_posesInTime.rows; ++i)
    {
        for (int j = 0; j < m_posesInTime.cols; ++j)
        {
            const cv::Vec3f &pos = m_posesInTime.at<cv::Vec3f>(i,j);

            const float xy = cv::norm(cv::Point2f(pos[0],pos[1]));
            const float xz = cv::norm(cv::Point2f(pos[0],pos[2]));
            const float yz = cv::norm(cv::Point2f(pos[1],pos[2]));

            channels[0].at<float>(i,j) = xy;
            channels[1].at<float>(i,j) = xz;
            channels[2].at<float>(i,j) = yz;
        }
    }

    for (size_t i = 0; i < channels.size(); ++i)
    {
        cv::Mat padded;
        int m = cv::getOptimalDFTSize(channels[i].rows);
        int n = cv::getOptimalDFTSize(channels[i].cols); // on the border add zero values
        cv::copyMakeBorder(channels[i], padded, 0, 0, 0, n - channels[i].cols, cv::BORDER_CONSTANT, cv::Scalar::all(0.0f));

        cv::Mat fd;
        cv::dft(padded,fd, cv::DFT_ROWS | cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);

        m_distanceFromCenterFd.push_back(fd);
    }
}
*/
