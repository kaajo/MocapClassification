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

#ifndef METRICVISUALIZATION_H
#define METRICVISUALIZATION_H

#include <QScrollArea>

#include <QVBoxLayout>

#include "visualization_global.h"


#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QLineSeries>

#include <mocapanimation.h>

namespace Ui {
class MetricVisualization;
}

class VISUALIZATIONSHARED_EXPORT MetricVisualization : public QScrollArea
{
    Q_OBJECT

public:
    explicit MetricVisualization(QWidget *parent = 0);
    ~MetricVisualization();

    void addAnimation(MocapAnimation *anim);
    void removeAnimation(MocapAnimation *anim);

private slots:
    void onCurDimChange(const QString &value);
    void onCurPointidChange(const QString &value);

private:
    Ui::MetricVisualization *ui;
    QVBoxLayout* m_layout;

    static const int numOfPoints;
    void refreshGraphs();
    QVector<MocapAnimation*> m_curAnims;
    int curDim = 0;
    int curPointId = 1;

    void addLine(QVBoxLayout *layout);

    void setupMovementQuantityGraph();
    void refreshMovementQuantityGraph();
    QtCharts::QChart *m_axisMovementQuantityChart = nullptr;
    QtCharts::QChartView *m_axisMovementQuantityVis = nullptr;
    QtCharts::QChart *m_movementQuantityChart = nullptr;
    QtCharts::QChartView *m_movementQuantityVis = nullptr;

    void setupAnimPropsGraph();
    void refreshAnimPropsGraph();
    QtCharts::QBarSeries *m_animPropsSeries = nullptr;
    QtCharts::QChart *m_animPropsChart = nullptr;
    QtCharts::QChartView *m_animPropsVis = nullptr;
};

#endif // METRICVISUALIZATION_H
