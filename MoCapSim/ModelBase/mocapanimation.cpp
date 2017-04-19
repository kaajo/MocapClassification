#include "mocapanimation.h"

#include <QtConcurrentMap>

#include <opencv2/imgproc/imgproc.hpp>

MocapAnimation::MocapAnimation(const int id, const int category, const QVector<MocapFrame> poses) :
    m_id(id),
    m_categoryId(category),
    m_posesInTime(poses[0].size(),poses.size(), CV_32FC3)
{
    for (int i = 0; i < poses.size(); ++i)
    {
        for (int j = 0; j < poses[0].size(); ++j)
        {
            m_posesInTime.at<cv::Vec3f>(j,i) = cv::Vec3f(poses[i][j].x(),poses[i][j].y(),poses[i][j].z());
        }
    }

    computeMovementQuantity();
    computeAxisMovementQuantity();
    computeVoxels();
    computeAxisFourierDescriptors();
    computeDFCFourierDescriptors();
}

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

QPair<float,MocapAnimation*> MocapAnimation::mapFun(MocapAnimation *it,const MocapAnimation *anim, MocapAnimation::SimilarityFunction function)
{
    return QPair<float,MocapAnimation*>(function(*anim,*it),it);
}

void MocapAnimation::computeMovementQuantity()
{
    for (size_t n = 0; n < 31; ++n)
    {
        float q = 0.0f;

        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            q += cv::norm(m_posesInTime.at<cv::Vec3f>(n,i) - m_posesInTime.at<cv::Vec3f>(n,i-1));
        }

        m_movementQuantity[n] = q;
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

void MocapAnimation::computeVoxels()
{
    for (int n = 0; n < m_posesInTime.rows; ++n)
    {
        for (int i = 1; i < m_posesInTime.cols; ++i)
        {
            const cv::Vec3f &pos = m_posesInTime.at<cv::Vec3f>(n,i);

            ++m_voxelMap[std::nearbyint(pos[0])][std::nearbyint(pos[1])][std::nearbyint(pos[2])];
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
