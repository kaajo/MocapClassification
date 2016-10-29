#include <QCoreApplication>

#include "functions.h"

#include <similaritymatrixcreator.h>
#include <modelfactory.h>

#include <QDir>
#include <QDebug>

int main(int /*argc*/, char *argv[])
{
    ModelFactory factory;
    QVector<MocapAnimation*> anims = factory.load("/home/kaajo/Dokumenty/SDIPR/data/objects-annotations-specific-coords_normPOS.data");

    qDebug() << "loaded " << anims.size() << "animations";

    SimilarityMatrixCreator creator;
    //creator.createCategoryImage(anims);

    for (int i = 1; i < anims[0][0][0].size(); ++i)
    {
        creator.createMetricImage(anims, std::bind(MetricFunctions::nodeMovementQuantity, std::placeholders::_1, i), argv[1], QString("nodeMovementQuantity") + QString::number(i));
    }

    //creator.createSimilarityImage(anims, SimilarityFunctions::pointDist, false);

    qDeleteAll(anims);

    return 0;
}
