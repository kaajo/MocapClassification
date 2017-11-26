#ifndef MOCAPMODEL_H
#define MOCAPMODEL_H

#include "bodynode.hpp"

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


#include <CImg.h>

class MocapAnimation
{
public:
    typedef QVector<QVector3D> MocapFrame;
    typedef std::function<float(const MocapAnimation&,const MocapAnimation&)> SimilarityFunction;
    typedef std::function<float(const MocapAnimation)> MetricFunction;
    typedef QMultiMap<float,QPair<int,MocapAnimation*>> Results;

    MocapAnimation(const int id,const int category,const QVector<MocapFrame> poses);
    ~MocapAnimation();

    int getId() const {return m_id;}
    void setId(const int id) {m_id = id;}
    int frames() const {return m_posesInTime.cols;}
    int16_t getRealCategory() const {return m_categoryId;}
    void setRealCategory(int16_t cat){m_categoryId = cat;}

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

    double PPScomputetotalMovementQuantity();
    double PPScomputeMovementQuantityAxisReduced();
    double PPScomputeAxisMovementQuantity();
    double PPScomputeMovementQuantityPoints();

    float getTotalMovementQuantity() const {return m_totalMovementQuantity;}
    cv::Vec3f getMovementQuantityAxisReduced() const {return m_movementQuantityAxisReduced;}
    std::array<float,31> getMovementQuantityPoints() const {return m_movementQuantityPoints;}
    std::array<cv::Vec3f, 31> getAxisMovementQuantity() const {return m_axisMovementQuantity;}
    std::array<cv::Vec3i,72> getAxisMovementDirectionHist() const {return m_axisMovementDirectionHist;}
    std::array<cv::Vec3f, 31> getAxisMovementAcc() const {return m_axisMovementAcc;}
    cimg_library::CImg<float> getVoxelMap() const {return m_voxelMap;}
    std::vector<cv::Mat> getAxisFourierDescriptor() const {return m_axisfd;}
    std::vector<cv::Mat> getDFCFourierDescriptor() const {return m_distanceFromCenterFd;}

    std::vector<std::array<BodyNode*,31> > getTreePosesInTime() const;

private:
    static QPair<float,MocapAnimation*> mapFun(MocapAnimation *it, const MocapAnimation*anim, MocapAnimation::SimilarityFunction function);

    int m_id = -1;

    int16_t m_categoryId = -1;

    /**
     * @brief first coord NODE id, second frame id
     */
    cv::Mat m_posesInTime;

    void createTreeStructure();
    std::vector<std::array<BodyNode*,31>> m_treePosesInTime;

    //////////////////

    void computeTotalMovementQuantity();
    float m_totalMovementQuantity = 0.0f;

    cv::Vec3f m_movementQuantityAxisReduced = cv::Vec3f(0.0f,0.0f,0.0f);
    void computeMovementQuantityAxisReduced();

    std::array<float,31> m_movementQuantityPoints;
    void computeMovementQuantityPoints();

    std::array<cv::Vec3f,31> m_axisMovementQuantity;
    void computeAxisMovementQuantity();

    std::array<cv::Vec3i,72> m_axisMovementDirectionHist; //xy,xz,yz
    void computeAxisMovementDirectionHist();

    std::array<cv::Vec3f,31> m_axisMovementAcc;
    void computeAxisMovementAcc();

    int m_voxelsPerMeter = 5;
    cimg_library::CImg<float> m_voxelMap = cimg_library::CImg<float>(20,20,20,1,0);
    void computeVoxels();

    std::vector<cv::Mat> m_axisfd;
    void computeAxisFourierDescriptors();

    std::vector<cv::Mat> m_distanceFromCenterFd;
    void computeDFCFourierDescriptors();
};

#endif // MOCAPMODEL_H
