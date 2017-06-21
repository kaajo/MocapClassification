#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadDataset("/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting/MoCapSim/objects-annotations-specific-coords_normPOS.data");

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

bool MainWindow::loadDataset(const QString &path, const int maxNOAnims)
{
    ModelFactory factory;
    m_anims = factory.load(path, maxNOAnims);

    if (m_anims.empty())
    {
        return false;
    }

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

    return true;
}

QVector<MocapAnimation *> MainWindow::anims() const
{
    return m_anims;
}
