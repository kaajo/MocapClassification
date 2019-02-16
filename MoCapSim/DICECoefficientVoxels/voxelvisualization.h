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

#include <QWidget>
#include <QVector>

#include <QtDataVisualization/q3dscatter.h>
#include <QtDataVisualization/qcustom3dvolume.h>

#include <CImg.h>

namespace Ui {
class VoxelVisualization;
}

class VoxelVisualization : public QWidget
{
    Q_OBJECT

public:
    explicit VoxelVisualization(QWidget *parent = nullptr);
    ~VoxelVisualization();

    void update(QHash<int,cimg_library::CImg<uint8_t>> imgs);

private:
    Ui::VoxelVisualization *ui;

    int m_voxelsPerMeter = 5;
    int voxelTilesX = 20;
    int voxelTilesY = 20;
    int voxelTilesZ = 20;

    void updateColorTable(const int noColors);

    QtDataVisualization::Q3DScatter *m_graph = nullptr;
    QVector<QColor> m_colorTable = {qRgba(0,0,0,0),qRgba(0,0,0,255)};
    QtDataVisualization::QCustom3DVolume *createVolume();
};
