#ifndef MOCAPMODEL_H
#define MOCAPMODEL_H

#include <QVector>
#include <QVector3D>
#include <QMap>
#include <QHash>
#include <QList>

#include <functional>

#include <opencv2/core/core.hpp>

/*
template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}
*/

class MocapAnimation
{
public:
    enum class NODE : int
    {
        ROOT = 1,
        LHIPJOINT = 2,
        LFEMUR = 3,
        LTIBIA = 4,
        LFOOT = 5,
        LTOES = 6,
        RHIPJOINT = 7,
        RFEMUR = 8,
        RTIBIA = 9,
        RFOOT = 10,
        RTOES = 11,
        LOWERBACK = 12,
        UPPERBACK = 13,
        THORAX = 14,
        LOWERNECK = 15,
        UPPERNECK = 16,
        HEAD = 17,
        LCLAVICLE = 18,
        LHUMERUS = 19,
        LRADIUS = 20,
        LWRIST = 21,
        LHAND = 22,
        LFINGERS = 23,
        LTHUMB = 24,
        RCLAVICLE = 25,
        RHUMERUS = 26,
        RRADIUS = 27,
        RWRIST = 28,
        RHAND = 29,
        RFINGERS = 30,
        RTHUMB = 31
    };

    typedef QVector<QVector3D> MocapFrame;
    typedef std::function<float(const MocapAnimation,const MocapAnimation)> SimilarityFunction;
    typedef std::function<float(const MocapAnimation)> MetricFunction;
    typedef QMultiMap<float,QPair<int,MocapAnimation*>> Results;

    MocapAnimation(const int id,const int category,const QVector<MocapFrame> poses);

    int getId() const {return m_id;}
    int frames() const {return m_posesInTime.cols;}
    int16_t getRealCategory() const {return m_categoryId;}

    QVector<QPair<float,MocapAnimation*>> getDistance(const QVector<QPair<float, MocapAnimation*>> &prevResults, const int topn,const MocapAnimation::SimilarityFunction function, cv::Mat &distanceMat) const;

    inline const cv::Vec3f& operator()(int node, int frame) const
    {
        assert(node >= 0);
        assert(frame >= 0);
        assert(node < m_posesInTime.rows);
        assert(frame < m_posesInTime.cols);

        return m_posesInTime.at<cv::Vec3f>(node,frame);
    }
    inline const cv::Mat operator()(int node) const
    {
        assert(node >= 0);
        assert(node < m_posesInTime.rows);

        return m_posesInTime.row(node);
    }

    std::array<float,31> getMovementQuantity() const {return m_movementQuantity;}
    std::array<cv::Vec3f, 31> getAxisMovementQuantity() const {return m_axisMovementQuantity;}
    QMap<int, QMap<int,QMap<int,int>>> getVoxelMap() const {return m_voxelMap;}
    std::vector<cv::Mat> getAxisFourierDescriptor() const {return m_axisfd;}
    std::vector<cv::Mat> getDFCFourierDescriptor() const {return m_distanceFromCenterFd;}

private:
    static QPair<float,MocapAnimation*> mapFun(MocapAnimation *it, const MocapAnimation*anim, MocapAnimation::SimilarityFunction function);

    const int m_id = -1;

    int16_t m_categoryId = -1;

    /**
     * @brief first coord NODE id, second frame id
     */
    cv::Mat m_posesInTime;

    //////////////////
    std::array<float,31> m_movementQuantity;
    void computeMovementQuantity();

    std::array<cv::Vec3f,31> m_axisMovementQuantity;
    void computeAxisMovementQuantity();

    QMap<int, QMap<int, QMap<int, int> > > m_voxelMap;
    void computeVoxels();

    std::vector<cv::Mat> m_axisfd;
    void computeAxisFourierDescriptors();

    std::vector<cv::Mat> m_distanceFromCenterFd;
    void computeDFCFourierDescriptors();
};

#endif // MOCAPMODEL_H
