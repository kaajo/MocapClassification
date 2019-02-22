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

#include "graphvisualization.h"
#include "ui_graphvisualization.h"

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

GraphVisualization::GraphVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphVisualization)
{
    ui->setupUi(this);
}

GraphVisualization::~GraphVisualization()
{
    delete ui;
}

void GraphVisualization::update(QHash<int, std::array<cv::Vec3f,31>> descriptors)
{
    QChart *chart = new QChart();
    chart->setTitle("Point movement quantity (every axis)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->createDefaultAxes();
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    for (auto &id : descriptors.keys())
    {
        QLineSeries *series = new QLineSeries();
        series->setName(QString::number(id));

        for (int j = 0; j < 31; ++j)
        {
            series->append(j*3,descriptors[id][j][0]);
            series->append(j*3+1,descriptors[id][j][1]);
            series->append(j*3+2,descriptors[id][j][2]);
        }

        chart->addSeries(series);
        chart->createDefaultAxes();
    }

    ui->chartView->setChart(chart);
}
