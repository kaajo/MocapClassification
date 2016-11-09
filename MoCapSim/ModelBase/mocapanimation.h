#ifndef MOCAPMODEL_H
#define MOCAPMODEL_H

#include <QVector>
#include <QVector3D>
#include <QMap>
#include <QHash>
#include <QList>

#include <functional>

/*
    public static final int LANDMARK_ROOT_ID = 1;
    public static final int LANDMARK_LHIPJOINT_ID = 2;
    public static final int LANDMARK_LFEMUR_ID = 3;
    public static final int LANDMARK_LTIBIA_ID = 4;
    public static final int LANDMARK_LFOOT_ID = 5;
    public static final int LANDMARK_LTOES_ID = 6;
    public static final int LANDMARK_RHIPJOINT_ID = 7;
    public static final int LANDMARK_RFEMUR_ID = 8;
    public static final int LANDMARK_RTIBIA_ID = 9;
    public static final int LANDMARK_RFOOT_ID = 10;
    public static final int LANDMARK_RTOES_ID = 11;
    public static final int LANDMARK_LOWERBACK_ID = 12;
    public static final int LANDMARK_UPPERBACK_ID = 13;
    public static final int LANDMARK_THORAX_ID = 14;
    public static final int LANDMARK_LOWERNECK_ID = 15;
    public static final int LANDMARK_UPPERNECK_ID = 16;
    public static final int LANDMARK_HEAD_ID = 17;
    public static final int LANDMARK_LCLAVICLE_ID = 18;
    public static final int LANDMARK_LHUMERUS_ID = 19;
    public static final int LANDMARK_LRADIUS_ID = 20;
    public static final int LANDMARK_LWRIST_ID = 21;
    public static final int LANDMARK_LHAND_ID = 22;
    public static final int LANDMARK_LFINGERS_ID = 23;
    public static final int LANDMARK_LTHUMB_ID = 24;
    public static final int LANDMARK_RCLAVICLE_ID = 25;
    public static final int LANDMARK_RHUMERUS_ID = 26;
    public static final int LANDMARK_RRADIUS_ID = 27;
    public static final int LANDMARK_RWRIST_ID = 28;
    public static final int LANDMARK_RHAND_ID = 29;
    public static final int LANDMARK_RFINGERS_ID = 30;
    public static final int LANDMARK_RTHUMB_ID = 31;
 */

class MocapAnimation
{
public:
    typedef QVector<QVector3D> MocapPose;
    typedef std::function<float(const MocapAnimation,const MocapAnimation)> SimilarityFunction;
    typedef std::function<float(const MocapAnimation)> MetricFunction;
    typedef QMultiMap<float,QPair<int,MocapAnimation*>> Results;

    MocapAnimation(int category,QVector<MocapPose> poses);

    static Results getResults(const QVector<MocapAnimation *> &anims, const int index, MocapAnimation::SimilarityFunction function, QVector<QVector<float> > &distanceMat);
    static Results getResults(const Results &prevResults, const int topn, const int index, const MocapAnimation *anim, MocapAnimation::SimilarityFunction function, QVector<QVector<float> > &distanceMat);

    float getMetric(const MetricFunction function) const;

    int size() const {return m_posesInTime.size();}

    const MocapPose operator[](int position) const;

    int16_t getRealCategory() const;

    QVector<MocapPose> getPosesInTime() const;

    std::array<float,31> getMovementQuantity() const {return m_movementQuantity;}

private:
    static QPair<float,QPair<int,MocapAnimation*>> mapFun(const QPair<int,MocapAnimation*> it,const MocapAnimation*anim, MocapAnimation::SimilarityFunction function);

    std::array<float,31> m_movementQuantity;
    void computeMovementQuantity();

    QHash<int,QHash<int,QHash<int,int>>> m_voxelMap;
    void computeVoxels();

    QVector<MocapPose> m_posesInTime;

    int16_t m_category = -1;
};

#endif // MOCAPMODEL_H
