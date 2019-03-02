#pragma once

#include <idistancefunction.h>

#include <QWidget>
#include <QFutureWatcher>

namespace Ui {
class PluginInfo;
}

class PluginInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PluginInfo(QSharedPointer<IDistanceFunction> plugin, QString iid, QWidget *parent = nullptr);
    ~PluginInfo();

    void setPlugin(QSharedPointer<IDistanceFunction> plugin);

private slots:
    void on_computeDesc_clicked();

    void on_computeDist_clicked();

private:
    Ui::PluginInfo *ui = nullptr;

    QSharedPointer<IDistanceFunction> m_plugin;
    QFutureWatcher<void> m_descWatcher;
    QFutureWatcher<void> m_distWatcher;
};
