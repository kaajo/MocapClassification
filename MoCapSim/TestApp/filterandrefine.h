/*
    Copyright (C) 2019  Miroslav Krajíček

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

#pragma once

#include "resultvisualization.hpp"

#include <mocapanimation.h>
#include <idistancefunction.h>
#include <result.h>

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
