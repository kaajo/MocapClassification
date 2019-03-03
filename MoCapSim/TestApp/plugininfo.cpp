#include "plugininfo.h"
#include "ui_plugininfo.h"

#include <opencv2/highgui.hpp>

PluginInfo::PluginInfo(PluginInfoSettings settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginInfo)
{
    ui->setupUi(this);
    ui->pluginName->setText(settings.iid);
    setPlugin(settings.plugin);

    m_pluginIid = settings.iid;
    m_saveDirectory = settings.saveDirectory;

    connect(&m_descWatcher,&QFutureWatcher<void>::progressValueChanged,ui->descriptorProgressBar,&QProgressBar::setValue);
    connect(&m_descWatcher,&QFutureWatcher<void>::progressRangeChanged,ui->descriptorProgressBar,&QProgressBar::setRange);

    connect(&m_distWatcher,&QFutureWatcher<void>::progressValueChanged,ui->distanceProgressBar,&QProgressBar::setValue);
    connect(&m_distWatcher,&QFutureWatcher<void>::progressRangeChanged,ui->distanceProgressBar,&QProgressBar::setRange);
    connect(&m_distWatcher,&QFutureWatcher<void>::finished,this,&PluginInfo::onComputeDistFinished);

    connect(this,&PluginInfo::requestVisUpdate,this,&PluginInfo::updateWidget);
    updateWidget();
}

PluginInfo::~PluginInfo()
{
    delete ui;
}

void PluginInfo::setDatasetName(const QString &datasetName)
{
    m_datasetName = datasetName;
    emit requestVisUpdate();
}

void PluginInfo::onComputeDistFinished()
{
    ui->computeDesc->setEnabled(true);

    if (m_saveDirectory.exists())
    {
        cv::imwrite(getDistanceMatrixPath().toStdString(),m_plugin->getDistanceMatrix());
    }

    emit requestVisUpdate();
}

void PluginInfo::updateWidget()
{
    ui->loadDistanceMatrix->setEnabled(QFile::exists(getDistanceMatrixPath()));
}

void PluginInfo::on_computeDesc_clicked()
{
    ui->computeDesc->setEnabled(false);
    m_descWatcher.setFuture(m_plugin->computeDescriptors());
    ui->computeDist->setEnabled(true);
}

void PluginInfo::on_computeDist_clicked()
{
    ui->computeDist->setEnabled(false);
    m_distWatcher.setFuture(m_plugin->computeAllDistances());
}

void PluginInfo::on_loadDistanceMatrix_clicked()
{
    cv::Mat distanceMatrix = cv::imread(getDistanceMatrixPath().toStdString(),cv::IMREAD_UNCHANGED);
    m_plugin->setDistanceMatrix(distanceMatrix);

    emit requestVisUpdate();
}

void PluginInfo::setPlugin(QSharedPointer<IDistanceFunction> plugin)
{
    m_plugin = plugin;

    auto vis = plugin->getVisualization();

    if (vis)
    {
        vis->setMinimumSize(1000,480);
        vis->setMaximumSize(1920,640);
        ui->horizontalLayout->addWidget(vis);
    }
}

QString PluginInfo::getDistanceMatrixPath()
{
    QString fixedPluginIid = m_pluginIid.replace('/','_');
    return m_saveDirectory.path() + "/" + fixedPluginIid + "_" + m_datasetName + ".exr";
}
