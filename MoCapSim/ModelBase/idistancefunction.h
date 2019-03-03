/*
    Copyright (C) 2019  Miroslav Krajíček

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

#pragma once

#include <QtPlugin>
#include <QFuture>

#include <mocapanimation.h>

#define DistanceFunctionInterface_iid "MotionCaptureClassification.DistanceFunctionInterface/1.0"

class IDistanceFunction {
public:
    virtual ~IDistanceFunction() = default;

    virtual void setAnimations(QVector<MocapAnimation*> animations) = 0;

    virtual QFuture<void> computeDescriptors() = 0;
    virtual QFuture<void> computeAllDistances() = 0;

    virtual QWidget *getVisualization() = 0;
    virtual void selectionAdded(int animId) = 0;
    virtual void selectionRemoved(int animId) = 0;

    cv::Mat getDistanceMatrix() const {return m_distanceMatrix;}
    void setDistanceMatrix(cv::Mat distMat) {m_distanceMatrix = distMat;}

protected:
    cv::Mat m_distanceMatrix;
};

Q_DECLARE_INTERFACE(IDistanceFunction,DistanceFunctionInterface_iid)
