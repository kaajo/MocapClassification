#include "weigtedmean.h"
#include "ui_weigtedmean.h"

WeigtedMean::WeigtedMean(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WeigtedMean)
{
    ui->setupUi(this);
}

WeigtedMean::~WeigtedMean()
{
    delete ui;
}
