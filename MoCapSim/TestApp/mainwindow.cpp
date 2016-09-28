#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dbscan.h"

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/QDirectionalLight>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>


#include <Qt3DExtras/QOrbitCameraController>

#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    view = new Qt3DExtras::Qt3DWindow();
    QWidget* container = QWidget::createWindowContainer(view);

    centralWidget()->layout()->addWidget(container);


    setup3DScene();

    ModelFactory factory;

    m_anims = factory.load("/home/kaajo/Dokumenty/SDIPR/data/objects-annotations-specific-coords_normPOS.data");    

    std::vector<std::vector<double>> m(m_anims.size(), std::vector<double>(m_anims.size(), 0.0));

    cv::Mat mat(m_anims.size(), m_anims.size(), CV_32FC1);

    for (unsigned int i=0; i < m_anims.size(); i++) {
        for (unsigned int j=0; j < m_anims.size(); j++) {
            // generate similarity
            double d = 0.0;//m_anims[i]->getError(*m_anims[j], pointDistance); //MocapAnimation::similarity(stats[i], stats[j]);
            double similarity = exp(-d);
            m[i][j] = similarity;
            m[j][i] = similarity;


            mat.at<float>(i,j) = similarity;
            mat.at<float>(j,i) = similarity;
        }
    }

    cv::imwrite("image.exr", mat);

    cv::Mat reim;

    reim = cv::imread("image.exr", CV_LOAD_IMAGE_ANYDEPTH);

    cv::Mat diff = mat != reim;

    qDebug() << reim.type();

    if (cv::countNonZero(diff) == 0)
    {
        qDebug() << "hovado";
    }

    makeClusters();

    //ref
    ref = m_anims[6];
    second = m_anims[7];


    if(!second)
    {
        qDebug() <<"omg";
    }


    // Material
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setDiffuse(QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f));

    Qt3DExtras::QPhongMaterial *material2 = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setDiffuse(QColor::fromRgbF(0.0f, 1.0f, 0.0f, 1.0f));

    int size = ref->operator[](0).size();

    for(int i = 0; i < size; ++i)
    {
        // Sphere
        Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
        sphereMesh->setRadius(0.3f);

        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();
        sphereTransform->setTranslation(ref->operator[](0)[i]);


        sphereEntity->addComponent(sphereMesh);
        sphereEntity->addComponent(material);
        sphereEntity->addComponent(sphereTransform);

        m_refTransforms.push_back(sphereTransform);
    }

    int size2 = second->operator[](0).size();

    for(int i = 0; i < size2; ++i)
    {
        // Sphere
        Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
        sphereMesh->setRadius(0.3f);

        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();
        sphereTransform->setTranslation(second->operator[](0)[i]);


        sphereEntity->addComponent(sphereMesh);
        sphereEntity->addComponent(material2);
        sphereEntity->addComponent(sphereTransform);

        m_transforms.push_back(sphereTransform);
    }

    /*
    qDebug() << "align error: " << second->allign(*ref, pointDist);
    qDebug() << " shift: " << second->getShift();
    */

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimer);
    m_timer->start(30);
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_anims);
    delete ui;
}

void MainWindow::drawUpdate()
{
    int size = ref->operator[](m_time).size();

    for(int i = 0; i < size; ++i)
    {
        m_refTransforms[i]->setTranslation(ref->operator[](m_time)[i]);

    }

    int size2 = second->operator[](m_time).size();

    for(int i = 0; i < size2; ++i)
    {
        m_transforms[i]->setTranslation(second->operator[](m_time)[i]);

    }

    if(m_time > ref->size() + ref->getShift() && m_time > second->size() + second->getShift())
    {
        m_time = 0;
    }
}

void MainWindow::onTimer()
{
    ++m_time;
    drawUpdate();
}

void MainWindow::setup3DScene()
{
    // Root entity
    rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *camera = view->camera();

    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 30.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    Qt3DCore::QEntity *camera_light_ent = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QDirectionalLight* camera_light = new Qt3DRender::QDirectionalLight();
    camera_light->setWorldDirection(QVector3D(0.0f, -0.5f, -1.0f));
    camera_light->setColor(QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f));
    camera_light_ent->addComponent(camera_light);

    view->setRootEntity(rootEntity);
}

void MainWindow::estimateCategories(QVector<MocapAnimation*> anims)
{
    QVector<QVector<MocapAnimation*>> categories;
    categories.push_back(QVector<MocapAnimation*>(1,anims[0]));

    double treshold = 0.00006;

    for(int i = 1; i < anims.size(); ++i)
    {
        double minError = std::numeric_limits<double>::max();
        int minErrorId = -1;

        for (int j = 0; j < categories.size(); ++j)
        {
            double error = 0.0; // anims[i]->/*getError(*categories[j][0],pointDist);//*/getError(*categories[j][0],pointDist);

            if(error < minError)
            {
                minErrorId = j;
                minError = error;
            }
        }

        anims[i]->setShift(0);
        if(minError < treshold)
        {
            categories[minErrorId].push_back(anims[i]);
        }
        else
        {
            categories.push_back(QVector<MocapAnimation*>(1,anims[i]));
        }
    }

    qDebug() << "processing stats for " << categories.size() << " categories";

    for(int i = 0; i < categories.size(); ++i)
    {
        for(int j = 0; j < categories[i].size(); ++j)
        {
            qDebug() << i << " " << categories[i][j]->getRealCategory();
        }
    }

    qDebug() << "finished";
}

void MainWindow::makeClusters()
{
    DbScan dbscan(m_anims,.0071,1);

    dbscan.run();

    auto groups = dbscan.getGroups();

    qDebug() << "group: " << groups.size();

    for (size_t i = 0; i < groups.size(); ++i)
    {
        qDebug() << "id: " << i;

        for (size_t j = 0; j < groups[i].size(); ++j)
        {
            qDebug() << "realID: " << groups[i][j]->getRealCategory();
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    estimateCategories(m_anims);
}
