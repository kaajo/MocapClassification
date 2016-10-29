#include "similaritymatrixcreator.h"

#include "modelfactory.h"

#include <QDebug>
#include <QDir>

SimilarityMatrixCreator::SimilarityMatrixCreator()
{

}

cv::Mat SimilarityMatrixCreator::createMatrix(const QVector<MocapAnimation *> &anims)
{
    cv::Mat retVal(1,anims.size(),CV_16U);

    for (int i = 0; i < anims.size(); ++i)
    {
        retVal.at<uint16_t>(i) = anims[i]->getRealCategory();
    }

    return retVal;
}

void SimilarityMatrixCreator::createCategoryImage(const QVector<MocapAnimation *> &anims, QString imageDirectory, QString imageName)
{
    cv::Mat categoryMat = createMatrix(anims);

    QString imagePath = imageDirectory + "/"+ imageName +".png";
    saveMatrix(imagePath, categoryMat);

    //test only
    cv::Mat b = loadMatrix(imagePath);
    b.convertTo(b, CV_16U);

    if (cv::countNonZero(categoryMat != b) == 0)
    {
        qInfo() << "Exported category image is equal";
    }
    else
    {
        qWarning() << "Exported category image is not equal";

        std::cout << "exported = "<< std::endl << " "  << categoryMat << std::endl << std::endl;
        std::cout << "imported = "<< std::endl << " "  << b << std::endl << std::endl;
    }
}

cv::Mat SimilarityMatrixCreator::createMatrix(const QVector<MocapAnimation *> &anims, MocapAnimation::MetricFunction function)
{
    cv::Mat retVal(1,anims.size(),CV_32FC1);

    for (int i = 0; i < anims.size(); ++i)
    {
        retVal.at<float>(i) = anims[i]->getMetric(function);
    }

    return retVal;
}

void SimilarityMatrixCreator::createMetricImage(const QVector<MocapAnimation *> &anims, MocapAnimation::MetricFunction function, QString imageDirectory, QString imageName)
{
    cv::Mat metricMat = createMatrix(anims, function);

    QString imagePath = imageDirectory + "/"+ imageName +".exr";
    saveMatrix(imagePath,metricMat);

    //test only
    cv::Mat b = loadMatrix(imagePath);

    if (cv::countNonZero(metricMat != b) == 0)
    {
        qInfo() << "Exported metric image is equal";
    }
    else
    {
        qWarning() << "Exported metric image is not equal";

        std::cout << "exported = "<< std::endl << " "  << metricMat << std::endl << std::endl;
        std::cout << "imported = "<< std::endl << " "  << b << std::endl << std::endl;
    }
}

cv::Mat SimilarityMatrixCreator::createMatrix(QVector<MocapAnimation *> anims, MocapAnimation::SimilarityFunction function, bool allign)
{
    cv::Mat retVal(anims.size(), anims.size(), CV_32FC1);

    for (int i = 0; i < anims.size(); ++i)
    {
        for (int j = 0; j <= i; ++j)
        {
            if (i == j)
            {
                retVal.at<float>(i,i) = 0.0;
            }
            else
            {
                retVal.at<float>(i,j) = retVal.at<float>(j,i) = function(*anims[i], *anims[j]);
            }
        }
    }

    return retVal;
}

void SimilarityMatrixCreator::createSimilarityImage(QVector<MocapAnimation*> anims, MocapAnimation::SimilarityFunction function, bool allign, QString imageDirectory, QString imageName)
{
    cv::Mat similarityMat = createMatrix(anims, function, allign);

    QString imagePath = imageDirectory + "/"+ imageName +".exr";
    saveMatrix(imagePath,similarityMat);

    //test only
    cv::Mat b = loadMatrix(imagePath);

    if (cv::countNonZero(similarityMat != b) == 0)
    {
        qInfo() << "Exported similarity image is equal";
    }
    else
    {
        qWarning() << "Exported similarity image is not equal";

        std::cout << "exported = "<< std::endl << " "  << similarityMat << std::endl << std::endl;
        std::cout << "imported = "<< std::endl << " "  << b << std::endl << std::endl;
    }
}

cv::Mat SimilarityMatrixCreator::loadMatrix(QString imageDirectory, QString imageName)
{
    QString imagePath = imageDirectory + "/"+ imageName;

    QFileInfo check_file(imagePath);

    if(!check_file.exists() || !check_file.isFile())
    {
        qWarning() << "file does not exist: " << imagePath;
        return cv::Mat();
    }

    return loadMatrix(imagePath);
}

void SimilarityMatrixCreator::saveMatrix(QString path, cv::Mat matrix)
{
    cv::imwrite(path.toStdString(),matrix);

    qDebug() << "image saved to: " << path;
}

cv::Mat SimilarityMatrixCreator::loadMatrix(QString path)
{
    return cv::imread(path.toStdString(), CV_LOAD_IMAGE_UNCHANGED);
}
