#include "mainwindow.h"

#include "functions.h"
#include "methodtester.h"

#include <QApplication>

#include <QObject>
#include <QDir>
#include <QTime>
#include <QFile>

#include <modelfactory.h>
#include <similaritymatrixcreator.h>

/*
#include <cuda_runtime.h>

#include <helper_functions.h>

extern "C"
int cuda_main(int argc, char *argv[],  std::vector<std::vector<std::vector<float3>>> anims);
*/

/*
bool areEqual(const cv::Mat& a, const cv::Mat& b) {
    cv::Mat temp;
    cv::bitwise_xor(a,b,temp); //It vectorizes well with SSE/NEON
    return !(cv::countNonZero(temp) );
}
*/

int main(int argc, char *argv[])
{
    srand(time(NULL));

    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    QVector<MocapAnimation*> anims = w.anims();

    SimilarityMatrixCreator creator;
    //creator.createSimilarityImage(anims,SimilarityFunctions::MDDDTWNorm,"/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting","MDDDTWNormNew");
    cv::Mat pointMovementCorrelationErrorMatrix = creator.loadMatrix("/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting","pointMovementCorrelation.exr");
    cv::Mat totalMovementRelativeErrorMatrix = creator.loadMatrix("/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting","totalMovementRelativeError.exr");
    cv::Mat discreteVoxelsMatrix = creator.loadMatrix("/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting","discreteVoxels1.exr");
    cv::Mat pointMovementAbsoluteErrorMatrix = creator.loadMatrix("/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting","pointMovementAbsoluteError.exr");
    cv::Mat MDDTWMatrix = creator.loadMatrix("/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting","MDDTW.exr");
    cv::Mat MDDDTWNormMatrix = creator.loadMatrix("/home/mkrajicek/Dokumenty/SDIPR/mocap-segmenting","MDDDTWNormNew.exr");

/*
    MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementDirectionHistogram,cv::Mat(),{1,2,5,10,15,20})});


    1: 55.2665 %
    2: 71.0021 %
    5: 84.7761 %
    10: 90.8742 %
    15: 93.3049 %
    20: 94.7548 %
*/


    //56 - 61 , 138 , 139
    //TODO úspešnosť kategorie

/*
    MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::compareHistFeatures,cv::Mat(),{1,2,5,10,15,20}),
                                    FunctionProp(50,SimilarityFunctions::MDDDTWNorm,MDDDTWNormMatrix,{1,2,5}),
                                    FunctionProp(40,SimilarityFunctions::compareHistFeatures,cv::Mat(),{1,2,5,10}),
                                    FunctionProp(30,SimilarityFunctions::MDDDTWNorm,MDDDTWNormMatrix,{1,2,5})});

    1: 87.2495 %
    2: 93.774 %
    5: 97.3561 %
*/

/*
    MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::compareHistFeatures,cv::Mat(),{1,2,5,10,15,20})});


1: 65.2452 %
2: 78.1237 %
5: 92.7932 %
10: 96.5032 %
15: 97.8252 %
20: 98.4648 %
*/

/*
    float max = 0.0f;
    float maxi = 0.0f,maxj = 0.0f,maxk = 0.0f;

    for (float k = 0.1f; k < 1.0f; k = k + 0.05f)
    for (float j = 0.0f; j < 1.0f; j = j + 0.05f)
    for (float i = 0.0f; i < 1.0f; i = i + 0.05f)
    {
        qDebug() << "weights: " << i << " " << j << " " << k;
        float err = MethodTester::testMethod(anims,{WeightedProp(i,MDDDTWNormMatrix),
                                        WeightedProp(j,pointMovementAbsoluteErrorMatrix),
                                        WeightedProp(k,discreteVoxelsMatrix)});

        qDebug() << err;

        if (err >= max)
        {
            max = err;
            maxi = i;
            maxj = j;
            maxk = k;
        }
    }

    qDebug() << "best score: " << max << " with coeffs: " << maxi << " " << maxj << " " << maxk << " ";
    //best score:  0.849041  with coeffs:  0.9   0.55   0.95
*/

    /*
    float max = 0.0f;
    float maxi = 0.0f,maxj = 0.0f,maxk = 0.0f;

    for (float k = 0.1f; k < 1.0f; k = k + 0.05f)
    for (float j = 0.0f; j < 1.0f; j = j + 0.05f)
    for (float i = 0.0f; i < 1.0f; i = i + 0.05f)
    {
        qDebug() << "weights: " << i << " " << j << " " << k;
        float err = MethodTester::testMethod(anims,{
                                        WeightedProp(i,totalMovementRelativeErrorMatrix),
                                        WeightedProp(j,pointMovementAbsoluteErrorMatrix),
                                        WeightedProp(k,MDDDTWNormMatrix)});

        if (err >= max)
        {
            max = err;
            maxi = i;
            maxj = j;
            maxk = k;
        }
    }

    qDebug() << "best score: " << max << " with coeffs: " << maxi << " " << maxj << " " << maxk << " ";

    */

    /*
    for (int i = 0; i < 47; ++i)
    {
        MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,pointMovementAbsoluteErrorMatrix,{47}),
                                        FunctionProp(47,SimilarityFunctions::MDDDTWNorm,MDDDTWNormMatrix,{i}),
                                        FunctionProp(i,SimilarityFunctions::discreteVoxels,discreteVoxelsMatrix,{1})});
    }
*/

/*
    MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,cv::Mat(),{1,5,10,15,20}),
                      FunctionProp(20,SimilarityFunctions::pointDistanceAlligned,cv::Mat(),{1,2,3})});
   1: 58.2516 %
    5: 86.3539 %
    10: 92.2388 %
    15: 94.371 %
    20: 95.6503 %
    ______________

1: 81.2793 %
2: 88.0597 %
3: 90.4478 %

*/


/*
    MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,cv::Mat(),{1,2,5,10,15,20}),
                      FunctionProp(15,SimilarityFunctions::MDDDTW,cv::Mat(),{1,2,3})});
    //81.7484 % only
*/

/*
    int Top = 50;
    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,{1,2,5,10,15,20,25,50}),
                      FunctionProp(top,SimilarityFunctions::MDDDTWNorm,{1,2,3})});

    TOP15
    1: 81.9616 %
    2: 87.5053 %
    3: 89.339 %

    TOP50
    1: 85.3305 %
    2: 92.0682 %
    3: 93.7313 %
*/


/*    MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::discreteVoxels,cv::Mat(),{1,2,5,10,15,20,25,50,100,200})});

1: 34.2857 %
2: 65.5011 %
5: 90.9595 %
10: 95.9915 %
15: 97.1855 %
20: 97.7825 %
25: 98.1237 %
50: 98.806 %
100: 99.3177 %
200: 99.7015 %

*/

/*
{
    auto start = std::chrono::steady_clock::now();
    float res;

    for (auto &anim : anims)
    {
        res = SimilarityFunctions::MDDDTWNorm(*anims[0],*anim);
    }

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> diff = end-start;
    std::cout << diff.count() << " result " << res << std::endl;
    }
    */

/*
    MethodTester::TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,{1,2,5,10,15,20,25,50}),
                                    FunctionProp(100,SimilarityFunctions::discreteVoxels,{1,2,5,10,15,20,25,50,100})});
*/
    //MethodTester::TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::MDDDTWNorm,{1,2,3})});// 15. chyba

    /*
    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementDominant,{1,5,10,15,20})});
    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,{1,5,10,15,20})});
*/

/*
    MethodTester::testMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,cv::Mat(),{1,5,10,15,20}),
                      FunctionProp(1000,SimilarityFunctions::pointMovementAbsoluteError,cv::Mat(),{1,5,10,15,20})});
    // 81.1514 %
*/

    return a.exec();
}
