#ifndef WEIGTEDMEAN_H
#define WEIGTEDMEAN_H

#include <QWidget>

namespace Ui {
class WeigtedMean;
}

class WeigtedMean : public QWidget
{
    Q_OBJECT

public:
    explicit WeigtedMean(QWidget *parent = nullptr);
    ~WeigtedMean();

private:
    Ui::WeigtedMean *ui;
};

#endif // WEIGTEDMEAN_H
