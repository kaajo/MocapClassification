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

void FilterAndRefine::addPlugin(QString name, QSharedPointer<IDistanceFunction> plugin)
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
        const cv::Mat newDist = m_tabs[i].plugin->getDistanceMatrix();

        for (const Result &res : resultVec)
        {
            const int id = res.animation()->getId();

            for (int j = 0; j < m_tabs[i].slider->value(); ++j)
            {
                const QPair<float, MocapAnimation*> pair = res.distance()[j];
                dist.at<float>(id,pair.second->getId()) = newDist.at<float>(id,pair.second->getId());
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
    QLabel *label = new QLabel("0");
    QSlider *slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setRange(0, m_tabs.empty() ? m_anims.size() : m_tabs.last().slider->value());
    connect(slider,&QSlider::valueChanged,label, QOverload<int>::of(&QLabel::setNum));
    connect(slider,&QSlider::sliderReleased,this, &FilterAndRefine::onSliderReleased);
    sliderLayout->addWidget(label);
    sliderLayout->addWidget(slider);

    mainLayout->addLayout(sliderLayout);

    ResultVisualization *resultVis = new ResultVisualization;
    tab->layout()->addWidget(resultVis);

    m_tabs.push_back({m_plugins[ui->pluginComboBox->currentText()],resultVis,slider});
    ui->tabWidget->addTab(tab,ui->pluginComboBox->currentText());
}

void FilterAndRefine::on_tabWidget_tabCloseRequested(int index)
{
    m_tabs.remove(index);
    ui->tabWidget->removeTab(index);
}
