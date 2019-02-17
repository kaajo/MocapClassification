#pragma once

#include "idistancefunction.h"

#include <QWidget>
#include <QFutureWatcher>

namespace Ui {
class PluginInfo;
}

class PluginInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PluginInfo(IDistanceFunction *plugin, QString iid, QWidget *parent = nullptr);
    ~PluginInfo();

    void setPlugin(IDistanceFunction *plugin);

private slots:
    void on_computeDesc_clicked();

    void on_computeDist_clicked();

private:
    Ui::PluginInfo *ui = nullptr;

    IDistanceFunction *m_plugin = nullptr;
    QFutureWatcher<void> m_descWatcher;
    QFutureWatcher<void> m_distWatcher;
};
