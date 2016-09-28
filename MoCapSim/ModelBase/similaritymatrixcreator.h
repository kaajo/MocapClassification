#ifndef SIMILARITYMATRIXCREATOR_H
#define SIMILARITYMATRIXCREATOR_H

#include "mocapanimation.h"

#include <QString>
#include <QVector>
#include <QDir>

#include <opencv2/opencv.hpp>

class SimilarityMatrixCreator
{
public:
    SimilarityMatrixCreator();

    // 1xN matrix
    cv::Mat createMatrix(const QVector<MocapAnimation*> &anims);

    void createCategoryImage(const QVector<MocapAnimation *> &anims, QString imageDirectory = QDir::currentPath(), QString imageName = "category");

    // 1xN matrix
    cv::Mat createMatrix(const QVector<MocapAnimation *> &anims, MocapAnimation::MetricFunction function);

    void createMetricImage(const QVector<MocapAnimation *> &anims, MocapAnimation::MetricFunction function, QString imageDirectory = QDir::currentPath(), QString imageName = "metric");

    // NxN matrix
    cv::Mat createMatrix(QVector<MocapAnimation*> anims, MocapAnimation::SimilarityFunction function, bool allign);

    void createSimilarityImage(QVector<MocapAnimation*> anims, MocapAnimation::SimilarityFunction function, bool allign = false, QString imageDirectory = QDir::currentPath(), QString imageName = "similarity");


    cv::Mat loadMatrix(QString imageDirectory, QString imageName);

private:
    // base functions
    void saveMatrix(QString path, cv::Mat matrix);
    cv::Mat loadMatrix(QString path);

};

#endif // SIMILARITYMATRIXCREATOR_H
