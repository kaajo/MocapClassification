#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>


class MocapAnimation;
class AnimPlayer;
class MetricVisualization;

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

    void loadHDM14(const QString& path, const int maxNOAnims = -1);
    void loadHDM65(const QString& path, const int maxNOAnims = -1);
    void loadHDM122(const QString& path, const int maxNOAnims = -1);

private slots:
    void animationChecked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    bool loadDataset(const QString &path,const int maxNOAnims = -1);

    QVector<MocapAnimation*> m_anims;
    MetricVisualization *m_vis = nullptr;
    AnimPlayer *m_animPlayer = nullptr;

    void addAnimsToUI();
    void filterAnims(QVector<int> skipCategories);

    void datasetStats();
};

#endif // MAINWINDOW_H
