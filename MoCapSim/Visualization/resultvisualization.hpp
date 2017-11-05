#ifndef RESULTVISUALIZATION_HPP
#define RESULTVISUALIZATION_HPP

#include <QWidget>

#include "visualization_global.h"

#include <resultstats.hpp>

namespace Ui {
class ResultVisualization;
}

class VISUALIZATIONSHARED_EXPORT ResultVisualization : public QWidget
{
    Q_OBJECT

public:
    explicit ResultVisualization(QWidget *parent = 0);
    ~ResultVisualization();

    void setResult(const ResultMetrics &result);

private:
    Ui::ResultVisualization *ui;

    void updateCharts(const ResultMetrics &m_result);

    qreal findMedian(QList<qreal> vec, int begin, int end);
    qreal findVariance(QList<qreal> vec);
};

#endif // RESULTVISUALIZATION_HPP
