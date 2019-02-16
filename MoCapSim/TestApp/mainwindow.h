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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>


class IDistanceFunction;
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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector<MocapAnimation *> anims() const {return m_anims;}

    void loadHDM14(const QString& path, const int maxNOAnims = -1);
    void loadHDM65(const QString& path, const int maxNOAnims = -1);
    void loadHDM122(const QString& path, const int maxNOAnims = -1);

private slots:
    void animationChecked(QListWidgetItem *item);

    void on_actionLoadPlugin_triggered();

private:
    Ui::MainWindow *ui;

    bool loadDataset(const QString &path,const int maxNOAnims = -1);

    QVector<MocapAnimation*> m_anims;
    MetricVisualization *m_vis = nullptr;
    AnimPlayer *m_animPlayer = nullptr;

    void addAnimsToUI();
    void filterAnims(QVector<int> skipCategories);

    void datasetStats();

    QVector<IDistanceFunction*> m_plugins;
};

#endif // MAINWINDOW_H
