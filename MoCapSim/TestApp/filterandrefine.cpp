#include "filterandrefine.h"
#include "ui_filterandrefine.h"

#include "methodtester.h"

#include <QLabel>

FilterAndRefine::FilterAndRefine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterAndRefine)
{
    ui->setupUi(this);
}

FilterAndRefine::~FilterAndRefine()
{
    delete ui;
}

void FilterAndRefine::addPlugin(QString name, IDistanceFunction *plugin)
{
    ui->pluginComboBox->addItem(name);

    m_plugins.insert(name,plugin);
}

void FilterAndRefine::setAnimations(QVector<MocapAnimation *> &anims)
{
    for (auto &anim : anims) {
        m_anims[anim->getId()] = anim;
    }
}

void FilterAndRefine::onSliderReleased()
{
    for (int i = 1; i < m_tabs.size(); ++i)
    {
        m_tabs[i].slider->setMaximum(m_tabs[i-1].slider->value());
    }

    cv::Mat dist = m_tabs[0].plugin->getDistanceMatrix();
    QVector<Result> resultVec = MethodTester::createResults(m_anims,dist,m_tabs[0].slider->value());
    m_tabs[0].visualization->setResult(ResultMetrics(resultVec));

    for (int i = 1; i < m_tabs.size(); ++i)
    {
        dist = cv::Mat(m_anims.size(),m_anims.size(),CV_32FC1, cv::Scalar(std::numeric_limits<float>::max()));
        cv::Mat newDist = m_tabs[i].plugin->getDistanceMatrix();

        for (const Result &res : resultVec)
        {
            const int id = res.animation()->getId();

            for (const QPair<float, MocapAnimation*> &pair : res.distance())
            {
                dist.at<float>(id,pair.second->getId()) = pair.first;
            }
        }

        resultVec = MethodTester::createResults(m_anims,dist,m_tabs[i].slider->value());
        m_tabs[i].visualization->setResult(ResultMetrics(resultVec));
    }
}

void FilterAndRefine::on_addStepButton_clicked()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout;
    tab->setLayout(mainLayout);

    QLayout *sliderLayout = new QHBoxLayout();
    QLabel *l = new QLabel("0");
    QSlider *s = new QSlider(Qt::Orientation::Horizontal);
    s->setRange(0, m_tabs.empty() ? m_anims.size() : m_tabs.last().slider->value());
    connect(s,&QSlider::valueChanged,l, QOverload<int>::of(&QLabel::setNum));
    connect(s,&QSlider::sliderReleased,this, &FilterAndRefine::onSliderReleased);
    sliderLayout->addWidget(l);
    sliderLayout->addWidget(s);

    mainLayout->addLayout(sliderLayout);

    ResultVisualization *resultVis = new ResultVisualization;
    tab->layout()->addWidget(resultVis);

    m_tabs.push_back({m_plugins[ui->pluginComboBox->currentText()],resultVis,s});
    ui->tabWidget->addTab(tab,ui->pluginComboBox->currentText());
}
