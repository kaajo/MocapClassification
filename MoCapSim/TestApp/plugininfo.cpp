#include "plugininfo.h"
#include "ui_plugininfo.h"

PluginInfo::PluginInfo(QSharedPointer<IDistanceFunction> plugin, QString iid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginInfo)
{
    ui->setupUi(this);
    ui->pluginName->setText(iid);
    setPlugin(plugin);

    connect(&m_descWatcher,&QFutureWatcher<void>::progressValueChanged,ui->descriptorProgressBar,&QProgressBar::setValue);
    connect(&m_descWatcher,&QFutureWatcher<void>::progressRangeChanged,ui->descriptorProgressBar,&QProgressBar::setRange);

    connect(&m_distWatcher,&QFutureWatcher<void>::progressValueChanged,ui->distanceProgressBar,&QProgressBar::setValue);
    connect(&m_distWatcher,&QFutureWatcher<void>::progressRangeChanged,ui->distanceProgressBar,&QProgressBar::setRange);
}

PluginInfo::~PluginInfo()
{
    delete ui;
}

void PluginInfo::setPlugin(QSharedPointer<IDistanceFunction> plugin)
{
    m_plugin = plugin;

    auto vis = plugin->getVisualization();

    if (vis)
    {
        vis->setMinimumSize(1000,480);
        ui->horizontalLayout->addWidget(vis);
    }
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
