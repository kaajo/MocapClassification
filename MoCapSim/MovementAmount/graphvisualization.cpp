#include "graphvisualization.h"
#include "ui_graphvisualization.h"

GraphVisualization::GraphVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphVisualization)
{
    ui->setupUi(this);
}

GraphVisualization::~GraphVisualization()
{
    delete ui;
}
