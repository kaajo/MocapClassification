#ifndef METRICVISUALIZATION_H
#define METRICVISUALIZATION_H

#include <QScrollArea>

#include <QVBoxLayout>

#include "visualization_global.h"

#include <mocapanimation.h>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>

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
    QtCharts::QBarSeries *m_movementQuantitySeries = nullptr;
    QtCharts::QChart *m_movementQuantityChart = nullptr;
    QtCharts::QChartView *m_movementQuantityVis = nullptr;

    void setupAnimPropsGraph();
    void refreshAnimPropsGraph();
    QtCharts::QBarSeries *m_animPropsSeries = nullptr;
    QtCharts::QChart *m_animPropsChart = nullptr;
    QtCharts::QChartView *m_animPropsVis = nullptr;

    void setupFourierDescGraph();
    void refreshFourierDescGraph();
    QtCharts::QBarSeries *m_fdSeries = nullptr;
    QtCharts::QChart *m_fdChart = nullptr;
    QtCharts::QChartView *m_fdVis = nullptr;

    void setupFourierDFCDescGraph();
    void refreshFourierDFCDescGraph();
    QtCharts::QBarSeries *m_fdDFCSeries = nullptr;
    QtCharts::QChart *m_fdDFCChart = nullptr;
    QtCharts::QChartView *m_fdDFCVis = nullptr;

};

#endif // METRICVISUALIZATION_H
