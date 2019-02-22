#ifndef FILTERANDREFINE_H
#define FILTERANDREFINE_H

#include "idistancefunction.h"

#include <mocapanimation.h>
#include <result.h>
#include <resultvisualization.hpp>

#include <QWidget>
#include <QSlider>

namespace Ui {
class FilterAndRefine;
}

class FilterAndRefine : public QWidget
{
    Q_OBJECT

public:
    explicit FilterAndRefine(QWidget *parent = nullptr);
    ~FilterAndRefine();

    void addPlugin(QString name, IDistanceFunction *plugin);

    void setAnimations(QVector<MocapAnimation*> &anims);

private slots:
    void onSliderReleased();

    void on_addStepButton_clicked();

    void on_tabWidget_tabCloseRequested(int index);

private:
    Ui::FilterAndRefine *ui;

    QHash<int,MocapAnimation*> m_anims;

    QHash<QString,IDistanceFunction*> m_plugins;

    struct TabData
    {
        IDistanceFunction *plugin = nullptr;
        ResultVisualization *visualization = nullptr;
        QSlider *slider = nullptr;
    };

    QVector<TabData> m_tabs;
};

#endif // FILTERANDREFINE_H
