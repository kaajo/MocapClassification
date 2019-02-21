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

#include "movementamount_global.h"
#include "graphvisualization.h"

#include "../TestApp/idistancefunction.h"

#include <QObject>
#include <QFutureWatcher>

class MOVEMENTAMOUNTSHARED_EXPORT MovementAmount : public QObject, public IDistanceFunction
{
    Q_OBJECT
    Q_INTERFACES(IDistanceFunction)
    Q_PLUGIN_METADATA(IID "MovementAmount.CVCompareHist/1.0")

public:
    MovementAmount(QObject *parent = nullptr);
    virtual ~MovementAmount() override = default;

    void setAnimations(QVector<MocapAnimation*> animations) override;

    QFuture<void> computeDescriptors() override;
    QFuture<void> computeAllDistances() override;
    float getDistance(MocapAnimation &anim1, MocapAnimation &anim2) override;

    QWidget* getVisualization()  override;
    void selectionAdded(int animId) override;
    void selectionRemoved(int animId) override;

private slots:
    void onComputeDescFinished();
    void onComputeDistAllFinished();

private:
    static float computeDist(std::array<cv::Vec3f,31> desc1, std::array<cv::Vec3f,31> desc2);

    static void computeDistToAll(const MocapAnimation *anim, cv::Mat &reducedResults,QVector<MocapAnimation*> anims,
                                 QHash<int,std::array<cv::Vec3f,31>> descriptors);

    QVector<MocapAnimation*> m_anims;

    QFutureWatcher<void> m_distanceWatcher;

    static QPair<int,std::array<cv::Vec3f,31>> computeDescriptor(const MocapAnimation * const anim);
    QFutureWatcher<QPair<int,std::array<cv::Vec3f,31>>> m_descWatcher;

    QHash<int,std::array<cv::Vec3f,31>> m_descriptors;

    QHash<int,std::array<cv::Vec3f,31>> m_selectedDescriptors;
    GraphVisualization *m_vis = new GraphVisualization();
    void refreshVis();
};
