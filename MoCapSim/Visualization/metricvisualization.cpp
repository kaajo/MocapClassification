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

#include <QLineEdit>
#include <QLabel>
#include <QSpacerItem>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>


#include "ui_metricvisualization.h"
#include "metricvisualization.h"


QT_CHARTS_USE_NAMESPACE

const int MetricVisualization::numOfPoints = 31;

MetricVisualization::MetricVisualization(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::MetricVisualization)
{
    ui->setupUi(this);

    m_layout = new QVBoxLayout();

    //setup graphs
    setupMovementQuantityGraph();
    addLine(m_layout);
    setupAnimPropsGraph();
    addLine(m_layout);

    auto widget = new QWidget();
    widget->setLayout(m_layout);
    setWidget(widget);
}

MetricVisualization::~MetricVisualization()
{
    delete ui;
}

void MetricVisualization::addAnimation(MocapAnimation *anim)
{
    m_curAnims.push_back(anim);
    refreshGraphs();
}

void MetricVisualization::removeAnimation(MocapAnimation *anim)
{
    m_curAnims.removeOne(anim);
    refreshGraphs();
}

void MetricVisualization::onCurDimChange(const QString &value)
{
    curDim = value.toInt();
    refreshGraphs();
}

void MetricVisualization::onCurPointidChange(const QString &value)
{
    curPointId = value.toInt();
    refreshGraphs();
}

void MetricVisualization::refreshGraphs()
{
    refreshMovementQuantityGraph();
    refreshAnimPropsGraph();
}

void MetricVisualization::addLine(QVBoxLayout *layout)
{
    QFrame *line;
    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    layout->addWidget(line);
}

void MetricVisualization::setupMovementQuantityGraph()
{
    m_axisMovementQuantityChart = new QChart();
    m_axisMovementQuantityChart->setTitle("Point movement quantity (every axis)");
    m_axisMovementQuantityChart->setAnimationOptions(QChart::SeriesAnimations);

    m_axisMovementQuantityChart->createDefaultAxes();
    m_axisMovementQuantityChart->legend()->setVisible(true);
    m_axisMovementQuantityChart->legend()->setAlignment(Qt::AlignBottom);

    m_axisMovementQuantityVis = new QChartView(m_axisMovementQuantityChart);
    m_axisMovementQuantityVis->setRenderHint(QPainter::Antialiasing);

    m_layout->addWidget(m_axisMovementQuantityVis);

    m_movementQuantityChart = new QChart();
    m_movementQuantityChart->setTitle("Point movement quantity (every point)");
    m_movementQuantityChart->setAnimationOptions(QChart::SeriesAnimations);

    m_movementQuantityChart->createDefaultAxes();
    m_movementQuantityChart->legend()->setVisible(true);
    m_movementQuantityChart->legend()->setAlignment(Qt::AlignBottom);

    m_movementQuantityVis = new QChartView(m_movementQuantityChart);
    m_movementQuantityVis->setRenderHint(QPainter::Antialiasing);

    m_layout->addWidget(m_movementQuantityVis);
}

void MetricVisualization::refreshMovementQuantityGraph()
{
    m_axisMovementQuantityChart->removeAllSeries();

    for (int i = 0; i < m_curAnims.size(); ++i)
    {
        QLineSeries *series = new QLineSeries();
        series->setName(QString::number(m_curAnims[i]->getId()));

        const std::array<cv::Vec3f,numOfPoints> mq = m_curAnims[i]->getAxisMovementQuantity();

        for (int j = 0; j < numOfPoints; ++j)
        {
            series->append(j*3,mq[j][0]);
            series->append(j*3+1,mq[j][1]);
            series->append(j*3+2,mq[j][2]);
        }

        m_axisMovementQuantityChart->addSeries(series);
        m_axisMovementQuantityChart->createDefaultAxes();
    }

    m_movementQuantityChart->removeAllSeries();
    for (int i = 0; i < m_curAnims.size(); ++i)
    {
        QLineSeries *series = new QLineSeries();
        series->setName(QString::number(m_curAnims[i]->getId()));

        const std::array<float,numOfPoints> mq = m_curAnims[i]->getMovementQuantityPoints();
        for (int j = 0; j < numOfPoints; ++j)
        {
            series->append(j,mq[j]);
        }

        m_movementQuantityChart->addSeries(series);
        m_movementQuantityChart->createDefaultAxes();
    }
}

void MetricVisualization::setupAnimPropsGraph()
{
    m_animPropsChart = new QChart();
    m_animPropsChart->setAnimationOptions(QChart::SeriesAnimations);

    m_animPropsSeries = new QBarSeries();
    m_animPropsSeries->setUseOpenGL(true);
    m_animPropsChart->addSeries(m_animPropsSeries);


    QStringList categories;
    categories << "total movement" << "movement X" << "movement Y" << "movement Z" << "frames";

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    m_animPropsChart->createDefaultAxes();
    m_animPropsChart->setAxisX(axis, m_animPropsSeries);

    m_animPropsChart->legend()->setVisible(true);
    m_animPropsChart->legend()->setAlignment(Qt::AlignBottom);

    m_animPropsVis = new QChartView(m_animPropsChart);
    m_animPropsVis->setRenderHint(QPainter::Antialiasing);

    m_layout->addWidget(m_animPropsVis);
}

void MetricVisualization::refreshAnimPropsGraph()
{
    m_animPropsChart->removeSeries(m_animPropsSeries);
    m_animPropsSeries->clear();

    for (int i = 0; i < m_curAnims.size(); ++i)
    {
        QBarSet *set = new QBarSet(QString::number(m_curAnims[i]->getId()));

        const std::array<float,numOfPoints> mq = m_curAnims[i]->getMovementQuantityPoints();
        float sum = std::accumulate(mq.begin(), mq.end(), 0.0f);

        const std::array<cv::Vec3f,numOfPoints> maq = m_curAnims[i]->getAxisMovementQuantity();
        cv::Vec3f sumAxis = std::accumulate(maq.begin(),maq.end(),cv::Vec3f(0.0f,0.0f,0.0f));

        *set << sum << sumAxis[0] << sumAxis[1] << sumAxis[2] << m_curAnims[i]->frames();
        m_animPropsSeries->append(set);
    }

    m_animPropsChart->addSeries(m_animPropsSeries);


    QStringList categories;
    categories << "total movement" << "movement X" << "movement Y" << "movement Z" << "frames";

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    m_animPropsChart->createDefaultAxes();
    m_animPropsChart->setAxisX(axis, m_animPropsSeries);
}
