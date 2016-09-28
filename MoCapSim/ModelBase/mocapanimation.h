#ifndef MOCAPMODEL_H
#define MOCAPMODEL_H

#include <QVector>
#include <QVector3D>
#include <QMap>
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
    typedef std::function<float(MocapAnimation,MocapAnimation)> SimilarityFunction;
    typedef std::function<float(const MocapAnimation)> MetricFunction;

    MocapAnimation(int category,QVector<MocapPose> poses);

    static float getError(MocapAnimation first, MocapAnimation second, SimilarityFunction function, bool allign = true);
    float getMetric(const MetricFunction function) const;

    int size() const {return m_posesInTime.size();}

    const MocapPose operator[](int position) const;

    int getShift() const;
    void setShift(int shift);

    int16_t getRealCategory() const;

    QVector<MocapPose> getPosesInTime() const;

private:
    static QMap<int, QList<int>> m_successors;

   //QVector<float> getMovementQuantity();

    QVector<MocapPose> m_posesInTime;
    int m_shift = 0;

    int16_t m_category = -1;
};

#endif // MOCAPMODEL_H
