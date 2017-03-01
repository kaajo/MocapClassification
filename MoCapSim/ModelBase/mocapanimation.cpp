#include "mocapanimation.h"

#include <QtConcurrentMap>

MocapAnimation::MocapAnimation(int category, QVector<MocapPose> poses, int id)
    : m_posesInTime(poses), m_category(category), m_id(id)
{
    computeMovementQuantity();
    computeVoxels();
}

MocapAnimation::Results MocapAnimation::getResults(const QVector<MocapAnimation*> &anims,const int index, MocapAnimation::SimilarityFunction function, QVector<QVector<float>> &distanceMat)
{
    QVector<QPair<int,MocapAnimation*>> animsReduced;

    for (int i = 0; i < anims.size(); ++i)
    {
        if (!(distanceMat[i][index] < std::numeric_limits<float>::max()) && i != index)
        {
            animsReduced.push_back({i,anims[i]});
        }
    }

    QVector<QPair<float,QPair<int,MocapAnimation*>>> tmpRes =
            QtConcurrent::blockingMapped<QVector<QPair<float,QPair<int,MocapAnimation*>>>>
            (animsReduced.begin(), animsReduced.end(),std::bind(mapFun,std::placeholders::_1,anims[index],function));

    for (int j = 0; j < animsReduced.size(); ++j)
    {
        distanceMat[index][tmpRes[j].second.first] = distanceMat[tmpRes[j].second.first][index] = tmpRes[j].first;
    }

    MocapAnimation::Results results;

    int i = 0;
    for (; i < index;++i)
    {
        results.insertMulti(distanceMat[index][i],{i,anims[i]});
    }
    ++i;
    for (;i < anims.size();++i)
    {
        results.insertMulti(distanceMat[index][i],{i,anims[i]});
    }

    return results;
}

MocapAnimation::Results MocapAnimation::getResults(const MocapAnimation::Results &prevResults, const int topn, const int index, const MocapAnimation *anim, MocapAnimation::SimilarityFunction function, QVector<QVector<float>> &distanceMat)
{
    QVector<QPair<int,MocapAnimation*>> resultsReduced;
    QVector<QPair<float,QPair<int,MocapAnimation*>>> resultsDone;

    for (int i = 0; i < prevResults.size() && i < topn; ++i)
    {
        if (!(distanceMat[(prevResults.begin()+i).value().first][index] < std::numeric_limits<float>::max()))
        {
            resultsReduced.push_back((prevResults.begin()+i).value());
        }
        else
        {
            resultsDone.push_back({distanceMat[(prevResults.begin()+i).value().first][index],(prevResults.begin()+i).value()});
        }
    }

    QVector<QPair<float,QPair<int,MocapAnimation*>>> tmpRes =
    QtConcurrent::blockingMapped<QVector<QPair<float,QPair<int,MocapAnimation*>>>>
            (resultsReduced.begin(),resultsReduced.end(),std::bind(mapFun,std::placeholders::_1,anim,function));

    MocapAnimation::Results results;
    for(int i = 0; i < tmpRes.size(); ++i)
    {
        distanceMat[index][tmpRes[i].second.first] = distanceMat[tmpRes[i].second.first][index] = tmpRes[i].first;
        results.insertMulti(tmpRes[i].first,tmpRes[i].second);
    }

    for(int i = 0; i < resultsDone.size(); ++i)
    {
        results.insertMulti(resultsDone[i].first,resultsDone[i].second);
    }

    return results;



    /*
    QVector<QPair<float,QPair<int,MocapAnimation*>>> tmpRes =
            QtConcurrent::blockingMapped<QVector<QPair<float,QPair<int,MocapAnimation*>>>>
            (prevResults.begin(),prevResults.begin() + topn,std::bind(mapFun,std::placeholders::_1,anim,function));


    MocapAnimation::Results results;
    for(int i = 0; i < tmpRes.size(); ++i)
    {
        distanceMat[index][tmpRes[i].second.first] = distanceMat[tmpRes[i].second.first][index] = tmpRes[i].first;
        results.insertMulti(tmpRes[i].first,tmpRes[i].second);
    }
    */

    return results;
}

QVector<QPair<float, MocapAnimation*>> MocapAnimation::getDistance(const QVector<MocapAnimation*> &anims, const int index, MocapAnimation::SimilarityFunction function, QVector<QVector<float> > &distanceMat)
{
    //optimisation - check if distance was computed before
    QVector<MocapAnimation*> animsReduced;
    QVector<QPair<float,MocapAnimation*>> distanceDone;
    for (int i = 0; i < anims.size(); ++i)
    {
        if (i == index) continue;

        if (distanceMat[i][index] >= std::numeric_limits<float>::max())
        {
            animsReduced.push_back(anims[i]);
        }
        else
        {
            distanceDone.push_back({distanceMat[i][index],anims[i]});
        }
    }

    QVector<QPair<float,MocapAnimation*>> distance =
            QtConcurrent::blockingMapped<QVector<QPair<float,MocapAnimation*>>>
            (animsReduced.begin(), animsReduced.end(),std::bind(mapFun2,std::placeholders::_1,anims[index],function));

    for (int j = 0; j < animsReduced.size(); ++j)
    {
        distanceMat[index][distance[j].second->m_id] =
        distanceMat[distance[j].second->m_id][index] = distance[j].first;
    }

    distance += distanceDone;
    return distance;
}

QVector<QPair<float, MocapAnimation*> > MocapAnimation::getDistance(const QVector<QPair<float, MocapAnimation*>> &prevResults, const int topn, const int index, const MocapAnimation *anim, MocapAnimation::SimilarityFunction function, QVector<QVector<float> > &distanceMat)
{
    QVector<MocapAnimation*> resultsReduced;
    QVector<QPair<float,MocapAnimation*>> resultsDone;

    for (int i = 0; i < prevResults.size() && i < topn; ++i)
    {
        if (distanceMat[prevResults[i].second->m_id][index] >= std::numeric_limits<float>::max())
        {
            resultsReduced.push_back(prevResults[i].second);
        }
        else
        {
            resultsDone.push_back({distanceMat[prevResults[i].second->m_id][index],prevResults[i].second});
        }
    }

    QVector<QPair<float,MocapAnimation*>> tmpRes =
    QtConcurrent::blockingMapped<QVector<QPair<float,MocapAnimation*>>>
            (resultsReduced.begin(),resultsReduced.end(),std::bind(mapFun2,std::placeholders::_1,anim,function));

    for(int i = 0; i < tmpRes.size(); ++i)
    {
        distanceMat[index][tmpRes[i].second->m_id] =
        distanceMat[tmpRes[i].second->m_id][index] = tmpRes[i].first;
    }

    tmpRes += resultsDone;
    return tmpRes;
}

float MocapAnimation::getMetric(const MetricFunction function) const
{
    return function(*this);
}

const MocapAnimation::MocapPose MocapAnimation::operator[](int position) const
{
    if(position >= size())
    {
        return m_posesInTime[size() - 1];
    }

    if(position < 0)
    {
        return m_posesInTime[0];
    }

    return m_posesInTime[position];
}

int16_t MocapAnimation::getRealCategory() const
{
    return m_category;
}

QVector<MocapAnimation::MocapPose> MocapAnimation::getPosesInTime() const
{
    return m_posesInTime;
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

        for (int i = 1; i < m_posesInTime.size(); ++i)
        {
            q += (m_posesInTime[i][n] - m_posesInTime[i-1][n]).length();
        }

        m_movementQuantity[n] = q;
    }
}

QMap<int, QMap<int, QMap<int, int> > > MocapAnimation::getVoxelMap() const
{
    return m_voxelMap;
}

void MocapAnimation::computeVoxels()
{
    for (int i = 1; i < m_posesInTime.size(); ++i)
    {
        for (size_t n = 0; n < 31; ++n)
        {
            const QVector3D &pos = m_posesInTime[i][n];
            //QVector3D voxel(floor(pos.x()),floor(pos.y()),floor(pos.z()));

           ++m_voxelMap[floor(pos.x())][floor(pos.y())][floor(pos.z())];
        }
    }
}
