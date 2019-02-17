#include "weigtedmean.h"
#include "ui_weigtedmean.h"

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

void WeigtedMean::addPlugin(QString name, IDistanceFunction *plugin)
{
    m_plugins.push_back(plugin);

    QTableWidgetItem *newItem = new QTableWidgetItem(name);
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount,0, newItem);

    QWidget *w = new QWidget();
    w->setLayout(new QHBoxLayout());

    QLabel *l = new QLabel("0");
    QSlider *s = new QSlider(Qt::Orientation::Horizontal);
    s->setRange(0,100);
    m_sliders.push_back(s);
    connect(s,&QSlider::valueChanged,l, QOverload<int>::of(&QLabel::setNum));
    connect(s,&QSlider::sliderReleased,this, &WeigtedMean::onSliderReleased);

    w->layout()->addWidget(l);
    w->layout()->addWidget(s);
    ui->tableWidget->setCellWidget(rowCount,1,w);
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

    cv::Mat res = m_plugins[0]->getDistanceMatrix() * m_sliders[0]->value();

    for (int i = 1; i < m_plugins.size(); ++i)
    {
        res += m_plugins[i]->getDistanceMatrix() * m_sliders[i]->value();
    }

    auto resultVec = createResults(res);
    m_resultVis->setResult(ResultMetrics(resultVec));
}

QVector<Result> WeigtedMean::createResults(cv::Mat distanceMatrix)
{
    cv::Mat indexes;
    cv::sortIdx(distanceMatrix,indexes,CV_SORT_EVERY_ROW | CV_SORT_ASCENDING);

    QVector<Result> results;

    for (int i = 0; i < indexes.rows; ++i)
    {
        QVector<QPair<float,MocapAnimation*>> dist;

        for (int col = 0; col < indexes.cols; ++col)
        {
            const int animIndex = indexes.at<int>(i,col);
            dist.push_back({distanceMatrix.at<float>(i,animIndex),m_anims[animIndex]});
        }

        Result r(m_anims[i],dist);
        results.push_back(r);
    }

    return results;
}
