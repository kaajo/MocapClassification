#include "metricvisualization.h"
#include "ui_metricvisualization.h"

#include <QLineEdit>
#include <QLabel>
#include <QSpacerItem>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

const int MetricVisualization::numOfPoints = 31;

MetricVisualization::MetricVisualization(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::MetricVisualization)
{
    ui->setupUi(this);

    m_layout = new QVBoxLayout();

    //setup buttons
    auto lineEditDim = new QLineEdit(QString::number(curDim));
    lineEditDim->setMaximumWidth(150);
    connect(lineEditDim,&QLineEdit::textChanged,this,&MetricVisualization::onCurDimChange);
    auto lineEditPointId = new QLineEdit(QString::number(curPointId));
    lineEditPointId->setMaximumWidth(150);
    connect(lineEditPointId,&QLineEdit::textChanged,this,&MetricVisualization::onCurPointidChange);

    auto hlayout = new QHBoxLayout();
    hlayout->addWidget(new QLabel("Dim: "));
    hlayout->addWidget(lineEditDim);
    hlayout->addWidget(new QLabel("PointID: "));
    hlayout->addWidget(lineEditPointId);
    hlayout->addSpacerItem(new QSpacerItem(1000,40,QSizePolicy::MinimumExpanding));
    m_layout->addLayout(hlayout);

    //setup graphs
    setupMovementQuantityGraph();
    addLine(m_layout);
    setupAnimPropsGraph();
    addLine(m_layout);
    setupFourierDescGraph();
    addLine(m_layout);
    setupFourierDFCDescGraph();
    addLine(m_layout);

    auto widget = new QWidget();
    widget->setLayout(m_layout);
    setWidget(widget);
}

MetricVisualization::~MetricVisualization()
{
    delete ui;
}

void MetricVisualization::addAnimation(MocapAnimation *anim)
{
    m_curAnims.push_back(anim);
    refreshGraphs();
}

void MetricVisualization::removeAnimation(MocapAnimation *anim)
{
    m_curAnims.removeOne(anim);
    refreshGraphs();
}

void MetricVisualization::onCurDimChange(const QString &value)
{
    curDim = value.toInt();
    refreshGraphs();
}

void MetricVisualization::onCurPointidChange(const QString &value)
{
    curPointId = value.toInt();
    refreshGraphs();
}

void MetricVisualization::refreshGraphs()
{
    refreshMovementQuantityGraph();
    refreshAnimPropsGraph();
    refreshFourierDescGraph();
    refreshFourierDFCDescGraph();
}

void MetricVisualization::addLine(QVBoxLayout *layout)
{
    QFrame *line;
    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    layout->addWidget(line);
}

void MetricVisualization::setupMovementQuantityGraph()
{
    m_movementQuantityChart = new QChart();
    m_movementQuantityChart->setTitle("Node movement quantity (%)");
    m_movementQuantityChart->setAnimationOptions(QChart::SeriesAnimations);

    m_movementQuantitySeries = new QBarSeries();
    m_movementQuantityChart->addSeries(m_movementQuantitySeries);

    m_movementQuantityChart->createDefaultAxes();
    m_movementQuantityChart->legend()->setVisible(true);
    m_movementQuantityChart->legend()->setAlignment(Qt::AlignBottom);

    m_movementQuantityVis = new QChartView(m_movementQuantityChart);
    m_movementQuantityVis->setRenderHint(QPainter::Antialiasing);

    m_layout->addWidget(m_movementQuantityVis);
}

void MetricVisualization::refreshMovementQuantityGraph()
{
    m_movementQuantityChart->removeSeries(m_movementQuantitySeries);
    m_movementQuantitySeries->clear();

    for (int i = 0; i < m_curAnims.size(); ++i)
    {
        QBarSet *set = new QBarSet(QString::number(m_curAnims[i]->getId()));

        const std::array<float,numOfPoints> mq = m_curAnims[i]->getMovementQuantity();

        for (int j = 0; j < numOfPoints; ++j)
        {
            *set <<  mq[j];
        }

        m_movementQuantitySeries->append(set);
    }
    m_movementQuantityChart->addSeries(m_movementQuantitySeries);
    m_movementQuantityChart->createDefaultAxes();
}

void MetricVisualization::setupAnimPropsGraph()
{
    m_animPropsChart = new QChart();
    m_animPropsChart->setTitle("Animations props");
    m_animPropsChart->setAnimationOptions(QChart::SeriesAnimations);

    m_animPropsSeries = new QBarSeries();
    m_animPropsChart->addSeries(m_animPropsSeries);


    QStringList categories;
    categories << "total movement" << "movement X" << "movement Y" << "movement Z" << "frames";

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    m_animPropsChart->createDefaultAxes();
    m_animPropsChart->setAxisX(axis, m_animPropsSeries);

    m_animPropsChart->legend()->setVisible(true);
    m_animPropsChart->legend()->setAlignment(Qt::AlignBottom);

    m_animPropsVis = new QChartView(m_animPropsChart);
    m_animPropsVis->setRenderHint(QPainter::Antialiasing);

    m_layout->addWidget(m_animPropsVis);
}

void MetricVisualization::refreshAnimPropsGraph()
{
    m_animPropsChart->removeSeries(m_animPropsSeries);
    m_animPropsSeries->clear();

    for (int i = 0; i < m_curAnims.size(); ++i)
    {
        QBarSet *set = new QBarSet(QString::number(m_curAnims[i]->getId()));

        const std::array<float,numOfPoints> mq = m_curAnims[i]->getMovementQuantity();
        float sum = std::accumulate(mq.begin(), mq.end(), 0.0f);

        const std::array<cv::Vec3f,numOfPoints> maq = m_curAnims[i]->getAxisMovementQuantity();
        cv::Vec3f sumAxis = std::accumulate(maq.begin(),maq.end(),cv::Vec3f(0.0f,0.0f,0.0f));

        *set << sum << sumAxis[0] << sumAxis[1] << sumAxis[2] << m_curAnims[i]->frames();
        m_animPropsSeries->append(set);
    }

    m_animPropsChart->addSeries(m_animPropsSeries);


    QStringList categories;
    categories << "total movement" << "movement X" << "movement Y" << "movement Z" << "frames";

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    m_animPropsChart->createDefaultAxes();
    m_animPropsChart->setAxisX(axis, m_animPropsSeries);
}

void MetricVisualization::setupFourierDescGraph()
{
    m_fdChart = new QChart();
    m_fdChart->setTitle("Fourier axis descriptors");
    m_fdChart->setAnimationOptions(QChart::SeriesAnimations);

    m_fdSeries = new QBarSeries();
    m_fdChart->addSeries(m_fdSeries);

    QStringList categories;

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    m_fdChart->createDefaultAxes();
    m_fdChart->setAxisX(axis, m_fdSeries);

    m_fdChart->legend()->setVisible(true);
    m_fdChart->legend()->setAlignment(Qt::AlignBottom);

    m_fdVis = new QChartView(m_fdChart);
    m_fdVis->setRenderHint(QPainter::Antialiasing);

    m_layout->addWidget(m_fdVis);
}

void MetricVisualization::refreshFourierDescGraph()
{
    m_fdChart->removeSeries(m_fdSeries);
    m_fdSeries->clear();

    for (int i = 0; i < m_curAnims.size(); ++i)
    {
        QBarSet *set = new QBarSet(QString::number(m_curAnims[i]->getId()));

        const cv::Mat fd = m_curAnims[i]->getAxisFourierDescriptor()[curDim];

        for (int j = 1; j < fd.cols && j < 50; ++j)
        {
            float val = fd.at<float>(curPointId,j);

            *set << val;
        }

        m_fdSeries->append(set);
    }

    m_fdChart->addSeries(m_fdSeries);
    m_fdChart->createDefaultAxes();
}

void MetricVisualization::setupFourierDFCDescGraph()
{
    m_fdDFCChart = new QChart();
    m_fdDFCChart->setTitle("Fourier DFC descriptors");
    m_fdDFCChart->setAnimationOptions(QChart::SeriesAnimations);

    m_fdDFCSeries = new QBarSeries();
    m_fdDFCChart->addSeries(m_fdDFCSeries);

    QStringList categories;

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    m_fdDFCChart->createDefaultAxes();
    m_fdDFCChart->setAxisX(axis, m_fdDFCSeries);

    m_fdDFCChart->legend()->setVisible(true);
    m_fdDFCChart->legend()->setAlignment(Qt::AlignBottom);

    m_fdDFCVis = new QChartView(m_fdDFCChart);
    m_fdDFCVis->setRenderHint(QPainter::Antialiasing);

    m_layout->addWidget(m_fdDFCVis);
}

void MetricVisualization::refreshFourierDFCDescGraph()
{
    m_fdDFCChart->removeSeries(m_fdDFCSeries);
    m_fdDFCSeries->clear();

    for (int i = 0; i < m_curAnims.size(); ++i)
    {
        QBarSet *set = new QBarSet(QString::number(m_curAnims[i]->getId()));

        const cv::Mat fd = m_curAnims[i]->getDFCFourierDescriptor()[curDim];

        for (int j = 1; j < fd.cols && j < 50; ++j)
        {
            float val = fd.at<float>(curPointId,j);

            *set << val;
        }

        m_fdDFCSeries->append(set);
    }

    m_fdDFCChart->addSeries(m_fdDFCSeries);
    m_fdDFCChart->createDefaultAxes();
}

