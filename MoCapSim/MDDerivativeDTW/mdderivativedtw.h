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

#include "mdderivativedtw_global.h"

#include <idistancefunction.h>

#include <QObject>
#include <QFutureWatcher>

class MDDERIVATIVEDTWSHARED_EXPORT MDDerivativeDTW : public QObject, public IDistanceFunction
{
    Q_OBJECT
    Q_INTERFACES(IDistanceFunction)
    Q_PLUGIN_METADATA(IID "MDDDTW.Normalized/1.0")

public:
    MDDerivativeDTW(QObject *parent = nullptr);
    virtual ~MDDerivativeDTW() override = default;

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
    static float computeDist(const MocapAnimation &first, const MocapAnimation &second);
    static float computeDistOpt(const MocapAnimation &first, const MocapAnimation &second, const cv::Mat &descFirst, const cv::Mat &descSecond);

    static QPair<int,cv::Mat> computeGradient(const MocapAnimation *const anim);
    QHash<int,cv::Mat> m_descriptors;
    QFutureWatcher<QPair<int,cv::Mat>> m_descWatcher;

    static void computeDistToAll(const MocapAnimation *anim,
                                 cv::Mat &reducedResults, QVector<MocapAnimation*> anims, QHash<int, cv::Mat> descriptors);
    QFutureWatcher<void> m_distanceWatcher;

    QVector<MocapAnimation*> m_anims;

    void refreshVis();
};
