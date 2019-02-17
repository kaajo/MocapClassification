#include "plugininfo.h"
#include "ui_plugininfo.h"

PluginInfo::PluginInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginInfo)
{
    ui->setupUi(this);
}

PluginInfo::~PluginInfo()
{
    delete ui;
}
