#pragma once

#include <mocapanimation.h>
#include <idistancefunction.h>
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

    QVector<QPair<QSlider*,IDistanceFunction*>> m_plugins;
    ResultVisualization *m_resultVis = new ResultVisualization;
};
