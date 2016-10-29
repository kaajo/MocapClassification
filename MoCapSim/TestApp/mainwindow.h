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

    void setAnims(MocapAnimation* ref, MocapAnimation* second);
    void setAnims(QVector<MocapAnimation*> anims) {m_anims = anims;}

signals:
    void timeChanged();

private slots:
    void onTimer();

    void on_refID_textChanged(const QString &arg1);
    void on_secID_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

    QVector<MocapAnimation*> m_anims;

    int m_time = 0;
    Qt3DExtras::Qt3DWindow* view = nullptr;

    Qt3DCore::QEntity *rootEntity;
    void setup3DScene();

    MocapAnimation* m_ref = nullptr, *m_second = nullptr;
    QVector<Qt3DCore::QTransform*> m_refTransforms, m_transforms;


    QTimer *m_timer = nullptr;
};

#endif // MAINWINDOW_H
