#include "weigtedmean.h"
#include "ui_weigtedmean.h"

#include "methodtester.h"

#include <resultstats.hpp>

#include <QTableWidgetItem>
#include <QLabel>

#include <opencv2/core.hpp>

WeigtedMean::WeigtedMean(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeigtedMean)
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(m_resultVis);
}

WeigtedMean::~WeigtedMean()
{
    delete ui;
}

void WeigtedMean::addPlugin(QString name, QSharedPointer<IDistanceFunction> plugin)
{
    QTableWidgetItem *newItem = new QTableWidgetItem(name);
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount,0, newItem);

    QWidget *w = new QWidget();
    w->setLayout(new QHBoxLayout());

    QLabel *l = new QLabel("0");
    QSlider *s = new QSlider(Qt::Orientation::Horizontal);
    s->setRange(0,100);
    connect(s,&QSlider::valueChanged,l, QOverload<int>::of(&QLabel::setNum));
    connect(s,&QSlider::sliderReleased,this, &WeigtedMean::onSliderReleased);

    w->layout()->addWidget(l);
    w->layout()->addWidget(s);
    ui->tableWidget->setCellWidget(rowCount,1,w);

    m_plugins.push_back({s,plugin});
}

void WeigtedMean::setAnimations(QVector<MocapAnimation *> &anims)
{
    for (auto &anim : anims) {
        m_anims[anim->getId()] = anim;
    }
}

void WeigtedMean::onSliderReleased()
{
    if (m_plugins.empty()) return;

    cv::Mat res = m_plugins[0].second->getDistanceMatrix() * m_plugins[0].first->value();

    for (int i = 1; i < m_plugins.size(); ++i)
    {
        res += m_plugins[i].second->getDistanceMatrix() * m_plugins[i].first->value();
    }

    auto resultVec = MethodTester::createResults(m_anims,res,m_anims.size());
    m_resultVis->setResult(ResultMetrics(resultVec));
}
