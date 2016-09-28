#ifndef CLUSTERING_H
#define CLUSTERING_H

#include "similaritymatrixcreator.h"

#include <QString>

#include <opencv2/opencv.hpp>

class Clustering
{
public:
    Clustering(const QString &dirPath);

private:
    SimilarityMatrixCreator creator;

    QVector<cv::Mat> m_metricMat;
    QVector<cv::Mat> m_similarityMat;
};

#endif // CLUSTERING_H
