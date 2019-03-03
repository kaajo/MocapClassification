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

#include "categorymapper.hpp"

#include "modelfactory.h"

#include <mocapanimation.h>
#include <idistancefunction.h>

#include <QFileDialog>
#include <QPluginLoader>
#include <QThread>
#include <QVBoxLayout>
#include <QDebug>
#include <QStandardPaths>
#include <QSettings>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_animPlayer = new AnimPlayer();
    ui->animPlayer->layout()->addWidget(m_animPlayer);

    m_weightedMean = new WeigtedMean;
    ui->WeightedMeanTab->layout()->addWidget(m_weightedMean);

    m_filterAndRefine = new FilterAndRefine;
    ui->FilterAndRefine->layout()->addWidget(m_filterAndRefine);

    //setup writable location
    QDir dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    const QString matrixFolder("DistanceMatrix");

    const bool saveDistMat = dataPath.mkpath(matrixFolder);
    dataPath.cd(matrixFolder);

    if (saveDistMat)
    {
        m_saveMatrixDir = dataPath;
        qDebug() << "Save location for distance matrices: " << dataPath.path();
    }
    else
    {
        qDebug() << "Could not create save location for distance matrices: " << dataPath.path();
    }

}

MainWindow::~MainWindow()
{
    qDeleteAll(m_anims);
    delete ui;
}

void MainWindow::loadHDM14(const QString &path, const int maxNOAnims)
{
    loadDataset(path,"HDM05-14",maxNOAnims);
    filterAnims({1,14});
    datasetStats();
    addAnimsToUI();
}

void MainWindow::loadHDM65(const QString &path, const int maxNOAnims)
{
    loadDataset(path,"HDM05-65",maxNOAnims);
    CategoryMapper::transform130to65(m_anims);
    datasetStats();
    addAnimsToUI();
}

void MainWindow::loadHDM122(const QString &path, const int maxNOAnims)
{
    loadDataset(path,"HDM05-122",maxNOAnims);
    filterAnims({56, 57,58,59,60, 61 , 138 , 139});
    datasetStats();
    addAnimsToUI();
}

void MainWindow::animationChecked(QListWidgetItem *item)
{
    const int id = item->data(Qt::UserRole).toInt();

    if (item->checkState() == Qt::CheckState::Checked)
    {
        m_animPlayer->addAnimation(m_anims[id]);
        std::for_each(m_plugins.begin(),m_plugins.end(),[id](auto p){p->selectionAdded(id);});
    }
    else
    {
        m_animPlayer->removeAnimation(m_anims[id]);
        std::for_each(m_plugins.begin(),m_plugins.end(),[id](auto p){p->selectionRemoved(id);});
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
        if (plugin)
        {
            QSharedPointer<IDistanceFunction> pluginObj(qobject_cast<IDistanceFunction*>(plugin));
            const QString pluginName = loader.metaData().find("IID")->toString("NOT FOUND");

            if (m_plugins.contains(pluginName))
            {
                continue;
            }

            qDebug() << "loading plugin:" << fileName;

            m_plugins.insert(pluginName,pluginObj);

            pluginObj->setAnimations(m_anims);

            PluginInfo *info = new PluginInfo({pluginObj,pluginName,m_saveMatrixDir});
            info->setDatasetName(m_datasetName);
            ui->scrollAreaWidgetContents->layout()->addWidget(info);

            m_pluginInfos.insert(pluginName,info);
            m_weightedMean->addPlugin(pluginName,pluginObj);
            m_filterAndRefine->addPlugin(pluginName,pluginObj);
        }
    }
}

void MainWindow::on_actionLoad_HDM14_triggered()
{
    const QString docLoc = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QSettings settings;
    const QString lastPath = settings.value("lastHDM14Path",docLoc).toString();

    const QString newPath = QFileDialog::getOpenFileName(this,"load HDM14 dataset",lastPath);
    settings.setValue("lastHDM14Path",newPath);

    loadHDM14(newPath);
}

void MainWindow::on_actionLoad_HDM65_triggered()
{
    const QString docLoc = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QSettings settings;
    const QString lastPath = settings.value("lastHDM65Path",docLoc).toString();

    const QString newPath = QFileDialog::getOpenFileName(this,"load HDM65 dataset",lastPath);
    settings.setValue("lastHDM65Path",newPath);

    loadHDM65(newPath);
}

void MainWindow::on_actionLoad_HDM122_triggered()
{
    const QString docLoc = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QSettings settings;
    const QString lastPath = settings.value("lastHDM122Path",docLoc).toString();

    const QString newPath = QFileDialog::getOpenFileName(this,"load HDM122 dataset",lastPath);
    settings.setValue("lastHDM122Path",newPath);

    loadHDM122(newPath);
}

bool MainWindow::loadDataset(const QString &path, const QString &datasetName, const int maxNOAnims)
{
    ModelFactory factory;
    qDeleteAll(m_anims);
    m_anims = factory.load(path, maxNOAnims);

    if (m_anims.empty())
    {
        return false;
    }

    m_datasetName = datasetName;

    for (auto &pluginInfo : m_pluginInfos)
    {
        pluginInfo->setDatasetName(m_datasetName);
    }

    return true;
}

void MainWindow::addAnimsToUI()
{
    ui->animsList->clear();

    for (int i = 0; i < m_anims.size(); ++i)
    {
        QListWidgetItem *item = new QListWidgetItem(QString::number(m_anims[i]->getId()) + " cat:" +
                                                    QString::number(m_anims[i]->getRealCategory()),
                                                    ui->animsList,QListWidgetItem::Type);

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole,i);
    }

    connect(ui->animsList, &QListWidget::itemChanged, this, &MainWindow::animationChecked,Qt::UniqueConnection);

    m_weightedMean->setAnimations(m_anims);
    m_filterAndRefine->setAnimations(m_anims);

    for (auto &plugin : m_plugins)
    {
        plugin->setAnimations(m_anims);
    }
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
