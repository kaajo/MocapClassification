#ifndef GRAPHVISUALIZATION_H
#define GRAPHVISUALIZATION_H

#include <QWidget>

namespace Ui {
class GraphVisualization;
}

class GraphVisualization : public QWidget
{
    Q_OBJECT

public:
    explicit GraphVisualization(QWidget *parent = nullptr);
    ~GraphVisualization();

private:
    Ui::GraphVisualization *ui;
};

#endif // GRAPHVISUALIZATION_H
