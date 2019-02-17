#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <QWidget>

namespace Ui {
class PluginInfo;
}

class PluginInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PluginInfo(QWidget *parent = nullptr);
    ~PluginInfo();

private:
    Ui::PluginInfo *ui;
};

#endif // PLUGININFO_H
