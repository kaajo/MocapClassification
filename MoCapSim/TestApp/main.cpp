#include "mainwindow.h"

#include "functions.h"

#include <QApplication>

#include <Qt3DInput/QAbstractPhysicalDevice>

#include <QObject>
#include <QDir>
#include <QTime>

#include <modelfactory.h>
#include <similaritymatrixcreator.h>

#include <iostream>
#include <iomanip>
#include <chrono>

using std::ostream;

/*
#include <cuda_runtime.h>

#include <helper_functions.h>

extern "C"
int cuda_main(int argc, char *argv[],  std::vector<std::vector<std::vector<float3>>> anims);
*/

//first is number of results from previous method, similarity function and set of "top N" results to print
typedef std::tuple<int,MocapAnimation::SimilarityFunction,QVector<int>> FunctionProp;

void printMethodError(int numOfResults, QVector<int> checkSet, QVector<float> output)
{
    std::cout << "used " << numOfResults << " from previous method" << std::endl;
    for(int i = 0; i < output.size(); ++i)
    {
        std::cout << checkSet[i] << ": " << output[i] * 100.0f << " %" << std::endl;
    }
    std::cout << "________________" << std::endl;
}

void printResult(int id, const MocapAnimation *const anim, const MocapAnimation::Results &result)
{
    if (anim->getRealCategory() == result.first().second->getRealCategory())
    {
        std::cout << id << " MATCH!" << std::endl;
    }
    else
    {
        std::cout << id << " MISS:  first category: " << anim->getRealCategory() << " second: " << result.first().second->getRealCategory() << " id: " << result.first().first << std::endl;
    }

}

QVector<float> checkMethodError(const QVector<MocapAnimation*> &anims,
                                const QVector<MocapAnimation::Results> &results,
                                const QVector<int> &numOfResults)
{
    int maxIter = *std::max_element(numOfResults.begin(),numOfResults.end());

    QVector<int> counters(numOfResults.size(), 0);

    for (int i = 0; i < anims.size(); ++i)
    {
        int n = 0;
        for (auto it = results[i].begin();it != results[i].begin() + maxIter; ++it, ++n)
        {
            if (it.value().second->getRealCategory() == anims[i]->getRealCategory())
            {
                for (int c = 0; c < numOfResults.size(); ++c)
                {
                    if (n < numOfResults[c])
                    {
                        ++counters[c];
                    }
                }
                break;
            }
        }
    }

    QVector<float> retVal(counters.size(), 0.0f);

    for (int i = 0; i < counters.size(); ++i)
    {
        retVal[i] = counters[i]/(float)anims.size();
    }

    return retVal;
}

void TestMethod(const QVector<MocapAnimation*> &anims,const QVector<FunctionProp> &functions)
{
    QVector<MocapAnimation::Results> results,resultsTmp;
    QVector<float> error;
    QVector<QVector<float>> distMatrix(anims.size(),QVector<float>(anims.size(),std::numeric_limits<float>::max()));

    //1st function (source is anims)
    for (int i = 0; i < anims.size(); ++i)
    {
        auto res = MocapAnimation::getResults(anims,i,std::get<1>(functions[0]),distMatrix);

        results.push_back(res);
        printResult(i,anims[i],res);
    }
    error = checkMethodError(anims,results,std::get<2>(functions[0]));
    printMethodError(std::get<0>(functions[0]),std::get<2>(functions[0]),error);

    for (int f = 1; f < functions.size(); ++f)
    {
        distMatrix = QVector<QVector<float>>(anims.size(),QVector<float>(anims.size(),std::numeric_limits<float>::max()));

        for (int i = 0; i < anims.size(); ++i)
        {
            auto res = MocapAnimation::getResults(results[i],std::get<0>(functions[f]),i,anims[i], std::get<1>(functions[f]),distMatrix);

            resultsTmp.push_back(res);
            printResult(i,anims[i],res);
        }
        results = resultsTmp;
        resultsTmp.clear();
        error = checkMethodError(anims,results,std::get<2>(functions[f]));
        printMethodError(std::get<0>(functions[f]),std::get<2>(functions[f]),error);
    }
}

template <class T, std::size_t N>
ostream& operator<<(ostream& o, const std::array<T, N>& arr)
{
    copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));
    return o;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qRegisterMetaType<Qt3DInput::QAbstractPhysicalDevice*>("'Qt3DInput::QAbstractPhysicalDevice*");

    //w.show();
    /*
    std::vector<std::vector<std::vector<float3>>> cuda_anims;

    // run your cuda application
    int curr = cuda_main(argc,argv,cuda_anims);

    QTime time;
    time.start();

    printf("CPU:%f ", anims[0]->allign(*(anims[1]), pointDistance));
    printf("elapsed: %d\n", time.elapsed());
*/


    ModelFactory factory;
    QVector<MocapAnimation*> anims = factory.load("/home/kaajo/Dokumenty/SDIPR/data/objects-annotations-specific-coords_normPOS.data");
    w.setAnims(anims);

    /*
    MocapAnimation* anim = anims[0];

    auto metrics = anim->getMovementQuantity();

    float sum = std::accumulate(metrics.begin(), metrics.end(), 0.0f);

    for (size_t n = 0; n < NUM_OF_NODES; ++n)
    {
        metrics[n] /= sum * 100.0f;
    }

    std::cout << std::setprecision(5)<< "ref " << metrics << std::endl;

    for (int i = 1; i < anims.size(); ++i)
    {
        if (anims[i]->getRealCategory() == anim->getRealCategory())
        {
            auto metrics = anims[i]->getMovementQuantity();

            float sum = std::accumulate(metrics.begin(), metrics.end(), 0.0f);

            for (size_t n = 0; n < NUM_OF_NODES; ++n)
            {
                metrics[n] /= sum * 100.0f;
            }

            std::cout << i << std::setprecision(5)<< " " << metrics << std::endl << std::endl;

        }

    }*/

/*
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
             float f = SimilarityFunctions::MDDDTWNorm(*anims[i],*anims[j]);
             float s = SimilarityFunctions::MDDDTWNormOpt(*anims[i],*anims[j]);
        }
    }

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> diff = end-start;
    std::cout <<  "elapsed : " << diff.count() << std::endl;

    start = std::chrono::steady_clock::now();

    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
             SimilarityFunctions::MDDDTWNormOpt(*anims[i],*anims[j]);
        }
    }

    end = std::chrono::steady_clock::now();
    diff = end-start;
    std::cout <<  "elapsed : " << diff.count() << std::endl;

*/
    /*
    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,{1,5,10,15,20}),
                      FunctionProp(20,SimilarityFunctions::pointDistanceAlligned,{1,2,3})});
    // 81.1514 %
    */

    /*
    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,{1,2,5,10,15,20}),
                      FunctionProp(15,SimilarityFunctions::MDDDTW,{1,2,3})});
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

    //TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::dominantPoints,{100,300})}); //cca 70%

    // TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::MDDDTWNorm,{1,2,3})}); 15. chyba

    /*
    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementDominant,{1,5,10,15,20})});
    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,{1,5,10,15,20})});
*/


    TestMethod(anims,{FunctionProp(anims.size(),SimilarityFunctions::pointMovementAbsoluteError,{1,5,10,15,20}),
                      FunctionProp(1000,SimilarityFunctions::pointMovementAbsoluteError,{1,5,10,15,20})});
    // 81.1514 %



    return 0;//a.exec();
}
