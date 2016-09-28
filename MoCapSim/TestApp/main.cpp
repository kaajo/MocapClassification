#include "mainwindow.h"

#include "clustering.h"
#include "dbscan.h"

#include <QApplication>

#include <Qt3DInput/QAbstractPhysicalDevice>

#include <QObject>
#include <QDir>
#include <QTime>

#include <modelfactory.h>
#include <similaritymatrixcreator.h>

#include <iostream>

/*
#include <cuda_runtime.h>

#include <helper_functions.h>

extern "C"
int cuda_main(int argc, char *argv[],  std::vector<std::vector<std::vector<float3>>> anims);
*/

int main(int argc, char *argv[])
{

    /*
    //QApplication a(argc, argv);
    //MainWindow w;

    //qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("'Qt3DInput::QAbstractPhysicalDevice*");

    //w.show();

    ModelFactory factory;
    QVector<MocapAnimation*> anims = factory.load("/home/kaajo/Dokumenty/SDIPR/data/objects-annotations-specific-coords_normPOS.data");



    std::vector<std::vector<std::vector<float3>>> cuda_anims;


    for (MocapAnimation* anim : anims)
    {
        std::vector<std::vector<float3>> tmp;

        QVector<QVector<QVector3D>> poses = anim->getPosesInTime();

        for (int i = 0; i < poses.size(); ++i)
        {
            std::vector<float3> tmp_int;

            for (int j = 0; j < poses[i].size(); ++j)
            {
                QVector3D &pose = poses[i][j];

                tmp_int.push_back(make_float3(pose.x(),pose.y(),pose.z()));
            }

            tmp.push_back(tmp_int);
        }

        cuda_anims.push_back(tmp);
    }


    // run your cuda application
    int curr = cuda_main(argc,argv,cuda_anims);

    QTime time;
    time.start();

    printf("CPU:%f ", anims[0]->allign(*(anims[1]), pointDistance));
    printf("elapsed: %d\n", time.elapsed());
*/
    //return a.exec();

    if (argc != 2)
    {
        qDebug() << "wrong arg count";
        return 100;
    }

    Clustering clustering(argv[1]);
/*
    ModelFactory factory;
    QVector<MocapAnimation*> anims = factory.load("/home/kaajo/Dokumenty/SDIPR/data/objects-annotations-specific-coords_normPOS.data");


        DbScan dbScan(anims,0.78,2);
        dbScan.setDistanceMatrix(similarity);

        dbScan.run();

        auto groups = dbScan.getGroups();

    for (size_t i = 0; i < groups.size(); ++i)
    {
        std::cout << std::endl << "group "<< i << std::endl << std::endl;

        for(size_t j = 0; j < groups[i].size(); ++j)
        {
            std::cout << "real category " << groups[i][j]->getRealCategory() << ", ";
        }
    }

*/
    return 0;
}
