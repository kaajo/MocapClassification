#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mocapanimation.h"
#include "modelfactory.h"

#include <metricvisualization.h>
#include <animplayer.h>

#include <QMainWindow>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QVector<MocapAnimation *> anims() const;

private slots:
    void animationChecked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    bool loadDataset(const QString &path,const int maxNOAnims = -1);

    QVector<MocapAnimation*> m_anims;
    MetricVisualization *m_vis = nullptr;
    AnimPlayer *m_animPlayer = nullptr;

    void datasetStats();
};

#endif // MAINWINDOW_H
