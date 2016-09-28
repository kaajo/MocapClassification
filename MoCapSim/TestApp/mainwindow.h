#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mocapanimation.h"
#include "modelfactory.h"

#include <QTimer>

#include <QMainWindow>
#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DCore/QTransform>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void drawUpdate();

signals:
    void timeChanged();

private slots:
    void onTimer();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QVector<MocapAnimation*> m_anims;

    int m_time = 0;
    Qt3DExtras::Qt3DWindow* view = nullptr;

    Qt3DCore::QEntity *rootEntity;
    void setup3DScene();

    MocapAnimation* ref = nullptr, *second = nullptr;
    QVector<Qt3DCore::QTransform*> m_refTransforms, m_transforms;


    QTimer *m_timer = nullptr;

    void estimateCategories(QVector<MocapAnimation *> anims);

    void makeClusters();

};

#endif // MAINWINDOW_H
