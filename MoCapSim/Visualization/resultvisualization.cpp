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

#include <QtCharts/QChartView>
#include <QtCharts/QBoxPlotSeries>
#include <QtCharts/QBoxSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>


#include "ui_resultvisualization.h"
#include "resultvisualization.hpp"

#include <cmath>

QT_CHARTS_USE_NAMESPACE

ResultVisualization::ResultVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultVisualization)
{
    ui->setupUi(this);
}

ResultVisualization::~ResultVisualization()
{
    delete ui;
}

void ResultVisualization::setResult(const ResultMetrics &result)
{
    updateCharts(result);
}

void ResultVisualization::updateCharts(const ResultMetrics &m_result)
{
    QMap<int16_t, qreal> res = m_result.getCategoryAccuracy();
    QList<qreal> sortedList = res.values();
    std::sort(sortedList.begin(),sortedList.end());

    int count = sortedList.size();
    qreal min = sortedList.first();
    qreal max = sortedList.last();
    qreal med = findMedian(sortedList, 0, count);
    qreal lowQ = findMedian(sortedList, 0, count / 2);
    qreal upQ = findMedian(sortedList , count / 2 + (count % 2), count);

    //histogram
    QLineSeries *series = new QLineSeries();
    for (const int16_t& key : res.keys())
    {
        series->append(QPointF(key,res[key]));
    }

    QChart *histChart = new QChart();
    histChart->legend()->hide();

    QValueAxis *aY = new QValueAxis();
    aY->setRange(min,max);
    aY->setTickCount(11);
    auto fY = aY->labelsFont();
    fY.setPixelSize(9);
    aY->setLabelsFont(fY);

    QValueAxis *aX = new QValueAxis();
    aX->setRange(res.firstKey(),res.lastKey());
    aX->setTickCount((res.lastKey() - res.firstKey())/10 + 1);
    aX->setMinorTickCount(10);
    aX->setLabelFormat("%d");
    auto fX = aX->labelsFont();
    fX.setPixelSize(9);
    aX->setLabelsFont(fX);

    histChart->setAxisX(aX);
    histChart->setAxisY(aY);
    histChart->addSeries(series);

    ui->histogramChart->setRenderHint(QPainter::Antialiasing);
    ui->histogramChart->setChart(histChart);

    //variance
    ui->variance->setText(QString::number(findVariance(res.values())));

    //accuracy
    ui->accuracy->setText(QString::number(m_result.getAccuracy()) + "%");

    //box-and-whiskers chart
    QBoxPlotSeries *catAccSeries = new QBoxPlotSeries();

    QBoxSet *set = new QBoxSet();
    set->setValue(QBoxSet::LowerExtreme, min);
    set->setValue(QBoxSet::UpperExtreme, max);
    set->setValue(QBoxSet::Median, med);
    set->setValue(QBoxSet::LowerQuartile, lowQ);
    set->setValue(QBoxSet::UpperQuartile, upQ);

    catAccSeries->append(set);

    QChart *chart = new QChart();
    chart->addSeries(catAccSeries);
    chart->legend()->hide();

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0.0,1.0);
    axisY->setTickCount(11);

    auto font = axisY->labelsFont();
    font.setPixelSize(9);
    axisY->setLabelsFont(font);

    chart->setAxisY(axisY);

    ui->boxPlotChart->setRenderHint(QPainter::Antialiasing);
    ui->boxPlotChart->setChart(chart);
}

qreal ResultVisualization::findMedian(QList<qreal> vec, int begin, int end)
{
    int count = end - begin;
    if (count % 2)
    {
        return vec.at(count / 2 + begin);
    }
    else
    {
        qreal right = vec.at(count / 2 + begin);
        qreal left = vec.at(count / 2 - 1 + begin);
        return (right + left) / 2.0;
    }
}

qreal ResultVisualization::findVariance(QList<qreal> vec)
{
    qreal sum = std::accumulate(vec.begin(), vec.end(), 0.0);
    qreal avg  = sum/static_cast<qreal>(vec.size());

    qreal temp = 0;
    for(double a : vec)
    {
        temp += std::pow(a-avg,2);
    }

    return temp/static_cast<qreal>(vec.size());
}
