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

    // NxN matrix
    cv::Mat createMatrix(QVector<MocapAnimation*> anims, MocapAnimation::SimilarityFunction function);

    void createSimilarityImage(QVector<MocapAnimation*> anims, MocapAnimation::SimilarityFunction function, QString imageDirectory = QDir::currentPath(), QString imageName = "similarity");


    cv::Mat loadMatrix(QString imageDirectory, QString imageName);

private:
    // base functions
    void saveMatrix(QString path, cv::Mat matrix);
    cv::Mat loadMatrix(QString path);

    static QPair<float,MocapAnimation*> mapFun(MocapAnimation *it, const MocapAnimation*anim, MocapAnimation::SimilarityFunction function);

};

#endif // SIMILARITYMATRIXCREATOR_H
