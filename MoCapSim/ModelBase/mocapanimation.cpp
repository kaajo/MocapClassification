#include "mocapanimation.h"

MocapAnimation::MocapAnimation(int category, QVector<MocapPose> poses)
    : m_posesInTime(poses), m_category(category)
{

}

QMap<int,QList<int>> MocapAnimation::m_successors  = {{1,{1,2}}};

float MocapAnimation::getMetric(const MetricFunction function) const
{
    return function(*this);
}

float MocapAnimation::getError(MocapAnimation first, MocapAnimation second, SimilarityFunction function, bool allign)
{
    if (!allign)
    {
        return  function(first, second);
    }

    int maxShift = (first.size() > second.size()) ? first.size() : second.size();

    float minError = std::numeric_limits<float>::max();
    int minErrorId = -1;
    for(int i = -maxShift; i < maxShift; ++i)
    {
        float error = function(first, second);

        if(error < minError)
        {
            minError = error;
            minErrorId = i;
        }
    }

    return minError;
}

const MocapAnimation::MocapPose MocapAnimation::operator[](int position) const
{
    int pos = position + m_shift;

    if(pos >= size())
    {
        return m_posesInTime[size() - 1];
    }

    if(pos < 0)
    {
        return m_posesInTime[0];
    }

    return m_posesInTime[pos];
}

int MocapAnimation::getShift() const
{
    return m_shift;
}

void MocapAnimation::setShift(int shift)
{
    m_shift = shift;
}

int16_t MocapAnimation::getRealCategory() const
{
    return m_category;
}

QVector<MocapAnimation::MocapPose> MocapAnimation::getPosesInTime() const
{
    return m_posesInTime;
}

/*
MocapAnimation::MocapAnimationStats MocapAnimation::getStats()
{
    MocapAnimationStats stats;
    stats.realCategory = this->m_category;

    QVector<QVector3D> lastVectors(31);

    //movement Quantity and node ratio
    for (int i = 1; i < m_posesInTime.size(); ++i)
    {
        for (int j = 0; j < m_posesInTime[j].size(); ++j)
        {
            QVector3D lastVec = lastVectors[j];
            QVector3D vec = (m_posesInTime[i][j] - m_posesInTime[i-1][j]);
            auto length = vec.length();

            if(!lastVectors[j].isNull())
            {
                auto angle = std::abs(std::atan2(QVector3D::crossProduct(lastVec,vec).length(),QVector3D::dotProduct(lastVec,vec)));

                stats.movementChangeNodeRatio[j] += angle;
                stats.movementChangeQuantity += angle;
            }

            lastVectors[j] = vec;

            stats.movementNodeRatio[j] += length;
            stats.movementQuantity += length;
        }
    }

    //norm
    for(int i = 0; i < stats.movementNodeRatio.size(); ++i)
    {
        stats.movementChangeNodeRatio[i] /= stats.movementChangeQuantity;
        stats.movementNodeRatio[i] /= stats.movementQuantity;
    }

    //movement Quantity change


    return stats;
}*/
