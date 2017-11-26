#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <animplayer.h>

#include <metricvisualization.h>
#include <mocapanimation.h>

#include "categorymapper.hpp"

#include <opencv2/opencv.hpp>
#include "modelfactory.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_vis = new MetricVisualization;
    ui->centralWidget->layout()->addWidget(m_vis);

    m_animPlayer = new AnimPlayer();
    m_animPlayer->show();
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_anims);
    delete m_vis;
    delete ui;
}

void MainWindow::animationChecked(QListWidgetItem *item)
{
    if (item->checkState() == Qt::CheckState::Checked)
    {
        m_vis->addAnimation(m_anims[item->data(Qt::UserRole).toInt()]);
        m_animPlayer->addAnimation(m_anims[item->data(Qt::UserRole).toInt()]);
    }
    else
    {
        m_vis->removeAnimation(m_anims[item->data(Qt::UserRole).toInt()]);
        m_animPlayer->removeAnimation(m_anims[item->data(Qt::UserRole).toInt()]);
    }
}

void MainWindow::loadHDM14(const QString &path, const int maxNOAnims)
{
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

QVector<MocapAnimation *> MainWindow::anims() const
{
    return m_anims;
}
