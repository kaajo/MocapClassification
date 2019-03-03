#pragma once

#include <idistancefunction.h>

#include <QWidget>
#include <QFutureWatcher>
#include <QDir>
#include <QUuid>

namespace Ui {
class PluginInfo;
}

struct PluginInfoSettings
{
    QSharedPointer<IDistanceFunction> plugin;
    QString iid;
    QDir saveDirectory;
};

class PluginInfo : public QWidget
{
    Q_OBJECT
public:
    explicit PluginInfo(PluginInfoSettings settings, QWidget *parent = nullptr);
    ~PluginInfo();

    void setDatasetName(const QString &datasetName);

signals:
    void requestVisUpdate();

private slots:
    void onComputeDistFinished();
    void updateWidget();

    void on_computeDesc_clicked();

    void on_computeDist_clicked();

    void on_loadDistanceMatrix_clicked();

private:
    Ui::PluginInfo *ui = nullptr;

    void setPlugin(QSharedPointer<IDistanceFunction> plugin);
    QString m_pluginIid = QUuid::createUuid().toString();
    QString m_datasetName;
    QSharedPointer<IDistanceFunction> m_plugin;
    QFutureWatcher<void> m_descWatcher;
    QFutureWatcher<void> m_distWatcher;

    QDir m_saveDirectory;
    QString getDistanceMatrixPath();
};
