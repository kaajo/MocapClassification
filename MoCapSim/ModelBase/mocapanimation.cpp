#include "mocapanimation.h"

#include <QtConcurrentMap>

#include <opencv2/imgproc/imgproc.hpp>

MocapAnimation::MocapAnimation(int category, QVector<MocapFrame> poses, int id)
    : m_id(id), m_categoryId(category)
{
    m_posesInTime.create(poses[0].size(),poses.size(), CV_32FC3);

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

QVector<QPair<float, MocapAnimation*>> MocapAnimation::getDistance(const QVector<MocapAnimation*> &anims, const int index, MocapAnimation::SimilarityFunction function, cv::Mat &distanceMat)
{
    //optimisation - check if distance was computed before
    QVector<MocapAnimation*> animsReduced;
    QVector<QPair<float,MocapAnimation*>> distanceDone;
    for (int i = 0; i < anims.size(); ++i)
    {
        if (i == index) continue;

        if (distanceMat.at<float>(i,index) >= std::numeric_limits<float>::max())
        {
            animsReduced.push_back(anims[i]);
        }
        else
        {
            distanceDone.push_back({distanceMat.at<float>(i,index),anims[i]});
/*
            float a = distanceMat.at<float>(i,index);
            float b = distanceMat.at<float>(index,i);
            float c = function(*anims[index],*anims[i]);
            float d = function(*anims[i],*anims[index]);

            if ((std::fabs(a - d)) > 0.01)
            {

                qDebug() << a << b << c << d;
            }
            */
        }
    }

    QVector<QPair<float,MocapAnimation*>> distance =
            QtConcurrent::blockingMapped<QVector<QPair<float,MocapAnimation*>>>
            (animsReduced.begin(), animsReduced.end(),std::bind(mapFun2,std::placeholders::_1,anims[index],function));

    for (int j = 0; j < animsReduced.size(); ++j)
    {
        distanceMat.at<float>(index,distance[j].second->m_id) =
                distanceMat.at<float>(distance[j].second->m_id,index) = distance[j].first;
    }

    distance += distanceDone;
    return distance;
}

QVector<QPair<float, MocapAnimation*> > MocapAnimation::getDistance(const QVector<QPair<float, MocapAnimation*>> &prevResults, const int topn, const int index, const MocapAnimation *anim, MocapAnimation::SimilarityFunction function, cv::Mat &distanceMat)
{
    QVector<MocapAnimation*> resultsReduced;
    QVector<QPair<float,MocapAnimation*>> resultsDone;

    for (int i = 0; i < prevResults.size() && i < topn; ++i)
    {
        if (distanceMat.at<float>(prevResults[i].second->m_id,index) >= std::numeric_limits<float>::max())
        {
            resultsReduced.push_back(prevResults[i].second);
        }
        else
        {
            resultsDone.push_back({distanceMat.at<float>(prevResults[i].second->m_id,index),prevResults[i].second});
        }
    }

    QVector<QPair<float,MocapAnimation*>> tmpRes =
            QtConcurrent::blockingMapped<QVector<QPair<float,MocapAnimation*>>>
            (resultsReduced.begin(),resultsReduced.end(),std::bind(mapFun2,std::placeholders::_1,anim,function));

    for(int i = 0; i < tmpRes.size(); ++i)
    {
        distanceMat.at<float>(index,tmpRes[i].second->m_id) =
                distanceMat.at<float>(tmpRes[i].second->m_id,index) = tmpRes[i].first;
    }

    tmpRes += resultsDone;
    return tmpRes;
}

QPair<float,QPair<int,MocapAnimation*>> MocapAnimation::mapFun(const QPair<int,MocapAnimation*> it,const MocapAnimation*anim, MocapAnimation::SimilarityFunction function)
{
    return QPair<float,QPair<int,MocapAnimation*>>(function(*anim,*it.second),it);
}

QPair<float,MocapAnimation*> MocapAnimation::mapFun2(MocapAnimation *it,const MocapAnimation *anim, MocapAnimation::SimilarityFunction function)
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
