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

    // Material
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setDiffuse(QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f));

    Qt3DExtras::QPhongMaterial *material2 = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setDiffuse(QColor::fromRgbF(0.0f, 1.0f, 0.0f, 1.0f));

    for(int i = 0; i < 31; ++i)
    {
        // Sphere
        Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
        sphereMesh->setRadius(0.3f);

        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();
        sphereTransform->setTranslation(QVector3D(0.0,0.0,0.0));


        sphereEntity->addComponent(sphereMesh);
        sphereEntity->addComponent(material);
        sphereEntity->addComponent(sphereTransform);

        m_refTransforms.push_back(sphereTransform);
    }

    for(int i = 0; i < 31; ++i)
    {
        // Sphere
        Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
        sphereMesh->setRadius(0.3f);

        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();
        sphereTransform->setTranslation(QVector3D(0.0,0.0,0.0));


        sphereEntity->addComponent(sphereMesh);
        sphereEntity->addComponent(material2);
        sphereEntity->addComponent(sphereTransform);

        m_transforms.push_back(sphereTransform);
    }

    /*
    qDebug() << "align error: " << second->allign(*ref, pointDist);
    qDebug() << " shift: " << second->getShift();
    */
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_anims);
    delete ui;
}

void MainWindow::drawUpdate()
{
    if (!m_ref || !m_second) return;

    int size = m_ref->operator[](m_time).size();

    for(int i = 0; i < size; ++i)
    {
        m_refTransforms[i]->setTranslation(m_ref->operator[](m_time)[i]);

    }

    int size2 = m_second->operator[](m_time).size();

    for(int i = 0; i < size2; ++i)
    {
        m_transforms[i]->setTranslation(m_second->operator[](m_time)[i]);

    }

    if(m_time > m_ref->size() && m_time > m_second->size())
    {
        m_time = 0;
    }
}

void MainWindow::setAnims(MocapAnimation *ref, MocapAnimation *second)
{
    m_ref = ref;
    m_second = second;

    if (m_timer) m_timer->deleteLater();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimer);
    m_timer->start(30);
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

void MainWindow::on_refID_textChanged(const QString &arg1)
{
    bool ok = false;
    int id = arg1.toInt(&ok);

    if (ok && id < m_anims.size())
    {
        setAnims(m_anims[id],m_second);
    }
}

void MainWindow::on_secID_textChanged(const QString &arg1)
{
    bool ok = false;
    int id = arg1.toInt(&ok);

    if (ok && id < m_anims.size())
    {
        setAnims(m_ref,m_anims[id]);
    }
}
