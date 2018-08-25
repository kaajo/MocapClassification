/*
    Copyright (C) 2017  Miroslav Krajíček

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <QDebug>
#include <QDir>
#include <QtConcurrent>


#include "modelfactory.h"

#include "similaritymatrixcreator.h"

#include "modelfactory.h"

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

cv::Mat SimilarityMatrixCreator::createMatrix(QVector<MocapAnimation *> anims, MocapAnimation::SimilarityFunction function)
{
    cv::Mat retVal(anims.size(), anims.size(), CV_32FC1);

    for (int i = 0 ; i < anims.size(); ++i)
    {
        retVal.at<float>(i,i) = 0.0f;
    }

    for (int i = 0; i < anims.size(); ++i)
    {
        QVector<QPair<float,MocapAnimation*>> result =
        QtConcurrent::blockingMapped<QVector<QPair<float,MocapAnimation*>>>
                (anims.begin(),anims.begin() + i,std::bind(mapFun,std::placeholders::_1,anims[i],function));

        for(int j = 0; j < result.size(); ++j)
        {
            retVal.at<float>(i,result[j].second->getId()) =
            retVal.at<float>(result[j].second->getId(),i) = result[j].first;
        }

        std::cout << "line " << i << std::endl;
    }

    return retVal;
}

void SimilarityMatrixCreator::createSimilarityImage(QVector<MocapAnimation*> anims, MocapAnimation::SimilarityFunction function, QString imageDirectory, QString imageName)
{
    cv::Mat similarityMat = createMatrix(anims, function);

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

QPair<float, MocapAnimation *> SimilarityMatrixCreator::mapFun(MocapAnimation *it, const MocapAnimation *anim, MocapAnimation::SimilarityFunction function)
{
    return QPair<float,MocapAnimation*>(function(*anim,*it),it);
}
