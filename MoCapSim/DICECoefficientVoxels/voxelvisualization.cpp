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

#include "ui_voxelvisualization.h"

#include "voxelvisualization.h"

#include <QMessageBox>
#include <QtDataVisualization/q3dinputhandler.h>


using namespace QtDataVisualization;

VoxelVisualization::VoxelVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VoxelVisualization)
{
    ui->setupUi(this);

    m_graph = new Q3DScatter();

    if (!m_graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
    }

    m_graph->activeTheme()->setType(Q3DTheme::ThemeQt);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    m_graph->setOrthoProjection(true);
    m_graph->activeTheme()->setBackgroundEnabled(false);

    static_cast<Q3DInputHandler*>(m_graph->activeInputHandler())->setZoomAtTargetEnabled(false);
    m_graph->scene()->activeCamera()->setMaxZoomLevel(200.0f);

    ui->verticalLayout->addWidget(QWidget::createWindowContainer(m_graph));
}

VoxelVisualization::~VoxelVisualization()
{
    delete ui;
}

void VoxelVisualization::update(QHash<int, cimg_library::CImg<uint8_t> > imgs)
{
    m_graph->removeCustomItems();

    updateColorTable(imgs.size());

    auto imgIt = imgs.begin();
    for (int i = 0; i < imgs.size(); ++i,imgIt++)
    {
        QVector<uchar> data = QVector<uchar>(20 * 20 * 20,0);

        for (uint x = 0; x < 20; ++x)
        {
            for (uint y = 0; y < 20; ++y)
            {
                for (uint z = 0; z < 20; ++z)
                {
                    data[x + 20*(y + 20*z)] = (*imgIt)(x,20-y,z);
                }
            }
        }

        auto volume = createVolume();
        volume->setColorTable({qRgba(0,0,0,0),m_colorTable[i].rgba()});
        volume->setTextureData(new QVector<uchar>(data));
        m_graph->addCustomItem(volume);
    }
}

void VoxelVisualization::updateColorTable(const int noColors)
{
    m_colorTable.resize(noColors);

    double hue = 0.0;
    for (int i = 0; i < noColors; ++i, hue += 0.9/noColors)
    {
        QColor color;
        color.setHsvF(hue,1.0,1.0,1.0/noColors);
        m_colorTable[i] = color;
    }
}

QCustom3DVolume* VoxelVisualization::createVolume()
{
    auto item = new QCustom3DVolume;

    item->setScalingAbsolute(false);
    item->setTextureFormat(QImage::Format_Indexed8);
    item->setPreserveOpacity(false);

    m_graph->axisX()->setRange(0.0f, 20.0f);
    m_graph->axisY()->setRange(0.0f, 20.0f);
    m_graph->axisZ()->setRange(0.0f, 20.0f);

    item->setTextureDimensions(20,20,20);
    item->setScaling({20,20,20});
    item->setPosition({10,10,10});

    return item;
}
