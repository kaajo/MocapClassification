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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPluginLoader>
#include <QThread>
#include <QVBoxLayout>
#include <QDebug>

#include <animplayer.h>

#include <metricvisualization.h>
#include <mocapanimation.h>

#include "idistancefunction.h"
#include "categorymapper.hpp"
#include "plugininfo.h"
#include "weigtedmean.h"
#include "filterandrefine.h"

#include "modelfactory.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_animPlayer = new AnimPlayer();
    ui->animPlayer->layout()->addWidget(m_animPlayer);

    m_vis = new MetricVisualization;
    ui->scrollAreaWidgetContents->layout()->addWidget(m_vis);

    m_weightedMean = new WeigtedMean;
    ui->WeightedMeanTab->layout()->addWidget(m_weightedMean);

    m_filterAndRefine = new FilterAndRefine;
    ui->FilterAndRefine->layout()->addWidget(m_filterAndRefine);
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_anims);
    delete m_vis;
    delete ui;
}

void MainWindow::animationChecked(QListWidgetItem *item)
{
    const int id = item->data(Qt::UserRole).toInt();

    if (item->checkState() == Qt::CheckState::Checked)
    {
        m_vis->addAnimation(m_anims[id]);
        m_animPlayer->addAnimation(m_anims[id]);
        std::for_each(m_plugins.begin(),m_plugins.end(),[id](IDistanceFunction *p){p->selectionAdded(id);});
    }
    else
    {
        m_vis->removeAnimation(m_anims[id]);
        m_animPlayer->removeAnimation(m_anims[id]);
        std::for_each(m_plugins.begin(),m_plugins.end(),[id](IDistanceFunction *p){p->selectionRemoved(id);});
    }
}

void MainWindow::loadHDM14(const QString &path, const int maxNOAnims)
{
    qDebug() << "";
    loadDataset(path,maxNOAnims);
    filterAnims({1,14});
    datasetStats();
    addAnimsToUI();
}

void MainWindow::loadHDM65(const QString &path, const int maxNOAnims)
{
    loadDataset(path,maxNOAnims);
    CategoryMapper::transform130to65(m_anims);
    datasetStats();
    addAnimsToUI();
}

void MainWindow::loadHDM122(const QString &path, const int maxNOAnims)
{
    loadDataset(path,maxNOAnims);
    filterAnims({56, 57,58,59,60, 61 , 138 , 139});
    datasetStats();
    addAnimsToUI();
}

bool MainWindow::loadDataset(const QString &path, const int maxNOAnims)
{
    ModelFactory factory;
    qDeleteAll(m_anims);
    m_anims = factory.load(path, maxNOAnims);

    if (m_anims.empty())
    {
        return false;
    }

    return true;
}

void MainWindow::addAnimsToUI()
{
    for (int i = 0; i < m_anims.size(); ++i)
    {
        QListWidgetItem *item = new QListWidgetItem(QString::number(m_anims[i]->getId()) + " cat:" +
                                                    QString::number(m_anims[i]->getRealCategory()),
                                                    ui->animsList,QListWidgetItem::Type);

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole,i);
    }

    connect(ui->animsList, &QListWidget::itemChanged, this, &MainWindow::animationChecked);

    m_weightedMean->setAnimations(m_anims);
    m_filterAndRefine->setAnimations(m_anims);
}

void MainWindow::filterAnims(QVector<int> skipCategories)
{
    for (int i = m_anims.size() - 1; i >= 0 ; --i)
    {
        if (skipCategories.contains(m_anims[i]->getRealCategory()))
        {
            m_anims.removeAt(i);
        }
    }

    for (int i = 0; i < m_anims.size(); ++i)
    {
        m_anims[i]->setId(i);
    }
}

void MainWindow::datasetStats()
{
    std::map<int,int> cats;

    float avgLength = 0.0f;

    for (int i= 0; i < m_anims.size(); ++i)
    {
        avgLength += m_anims[i]->frames();
        cats[m_anims[i]->getRealCategory()]++;
    }

    avgLength /= static_cast<float>(m_anims.size());

    std::cout << "Dataset: " << m_anims.size() << " actions with "
              << cats.size() << " categories" << std::endl;
    std::cout << "avg length of action: " << avgLength << std::endl;
    std::cout << "category , count" << std::endl;

    for (std::pair<int,int> cat : cats) {
        std::cout << cat.first << " , " << cat.second << std::endl;
    }
}

void MainWindow::on_actionLoadPlugin_triggered()
{
    const QDir pluginsDir = QDir(QFileDialog::getExistingDirectory(this, tr("Load plugins from directory"),QApplication::applicationDirPath()));

    if (pluginsDir.isEmpty()) return;

    const auto entryList = pluginsDir.entryList(QDir::Files);
    for (const QString &fileName : entryList) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            qDebug() << "loaded plugin:" << fileName;
            auto object = qobject_cast<IDistanceFunction*>(plugin);
            object->setAnimations(m_anims);
            m_plugins.push_back(object);

            const QString pluginName = loader.metaData().find("IID")->toString("NOT FOUND");

            PluginInfo *info = new PluginInfo(object,pluginName);
            ui->scrollAreaWidgetContents->layout()->addWidget(info);

            m_weightedMean->addPlugin(pluginName,object);
            m_filterAndRefine->addPlugin(pluginName,object);
        }
    }
}
