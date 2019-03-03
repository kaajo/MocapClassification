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

#include "dicecoefficientvoxels_global.h"
#include "voxelvisualization.h"

#include <idistancefunction.h>

#include <QObject>
#include <QFutureWatcher>

class DICECOEFFICIENTVOXELSSHARED_EXPORT DICECoefficientVoxels : public QObject, public IDistanceFunction
{
    Q_OBJECT
    Q_INTERFACES(IDistanceFunction)
    Q_PLUGIN_METADATA(IID "VOXELS.DICE/1.0")

public:
    DICECoefficientVoxels(QObject *parent = nullptr);
    virtual ~DICECoefficientVoxels() override = default;

    void setAnimations(QVector<MocapAnimation*> animations) override;

    QFuture<void> computeDescriptors() override;
    QFuture<void> computeAllDistances() override;

    QWidget* getVisualization()  override;
    void selectionAdded(int animId) override;
    void selectionRemoved(int animId) override;

private slots:
    void onComputeDescFinished();

private:
    static float computeDist(const cimg_library::CImg<uint8_t> &desc1, const cimg_library::CImg<uint8_t> &desc2);

    QVector<MocapAnimation*> m_anims;

    struct DescriptorSettings
    {
        int m_voxelsPerMeter = 5;
        unsigned int voxelTilesX = 20;
        unsigned int voxelTilesY = 20;
        unsigned int voxelTilesZ = 20;
    };

    static void computeDistToAll(const MocapAnimation *anim, cv::Mat &reducedResults,QVector<MocapAnimation*> anims, QHash<int,cimg_library::CImg<uint8_t>> descriptors);
    QFutureWatcher<void> m_distanceWatcher;

    QHash<int,cimg_library::CImg<uint8_t>> m_descriptors;
    static QPair<int, cimg_library::CImg<uint8_t> > computeVoxels(const MocapAnimation * const anim);
    QFutureWatcher<QPair<int,cimg_library::CImg<uint8_t>>> m_descWatcher;

    VoxelVisualization *m_vis = new VoxelVisualization();
    QHash<int,cimg_library::CImg<uint8_t>> m_visImgs;
    void refreshVis();
};


