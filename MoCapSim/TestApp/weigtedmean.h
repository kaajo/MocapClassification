#pragma once

#include "idistancefunction.h"

#include <mocapanimation.h>
#include <result.h>
#include <resultvisualization.hpp>

#include <QWidget>
#include <QSlider>

namespace Ui {
class WeigtedMean;
}

class WeigtedMean : public QWidget
{
    Q_OBJECT

public:
    explicit WeigtedMean( QWidget *parent = nullptr);
    ~WeigtedMean();

    void addPlugin(QString name, IDistanceFunction *plugin);

    void setAnimations(QVector<MocapAnimation*> &anims);

private slots:
    void onSliderReleased();

private:
    Ui::WeigtedMean *ui;

    QHash<int,MocapAnimation*> m_anims;

    QVector<IDistanceFunction*> m_plugins;
    QVector<QSlider*> m_sliders;

    QVector<Result> createResults(cv::Mat distanceMatrix);

    ResultVisualization *m_resultVis = new ResultVisualization;
};
