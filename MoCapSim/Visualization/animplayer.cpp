#include <QLayout>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QDirectionalLight>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DInput/QAbstractPhysicalDevice>

#include "ui_animplayer.h"
#include "animplayer.h"


AnimPlayer::AnimPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimPlayer)
{
    ui->setupUi(this);

    view = new Qt3DExtras::Qt3DWindow();
    QWidget* container = QWidget::createWindowContainer(view,this);
    ui->viewLayout->addWidget(container);

    // Root entity
    rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);

    setupCamera();

    connect(ui->slider,&QSlider::valueChanged,this,&AnimPlayer::sliderFrameChanged);
}

AnimPlayer::~AnimPlayer()
{
    qDeleteAll(m_animations);
    delete ui;
}

void AnimPlayer::addAnimation(MocapAnimation *anim)
{
    MocapAnimation3D *a = new MocapAnimation3D;
    a->animation = anim;

    int r = ((rand()+m_animations.size()*10)%128)+128;
    int g = ((rand()+m_animations.size()*10)%128)+128;
    int b = ((rand()+m_animations.size()*10)%128)+128;

    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setDiffuse(QColor::fromRgb((rand()%2)*r,g,b,128));

    std::vector<std::array<BodyNode*,31> > tree = anim->getTreePosesInTime();

    for(int i = 0; i < 31; ++i)
    {
        // Sphere
        Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
        sphereMesh->setRadius(0.23f);
/*
        // Bone
        Qt3DCore::QEntity *boneEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QCylinderMesh *boneMesh = new Qt3DExtras::QCylinderMesh;
        boneMesh->setRadius(0.1f);
*/
        Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();
        sphereTransform->setTranslation(QVector3D(0.0,0.0,0.0));

        sphereEntity->addComponent(sphereMesh);
        sphereEntity->addComponent(material);
        sphereEntity->addComponent(sphereTransform);
/*
        boneEntity->addComponent(boneMesh);
        boneEntity->addComponent(material);
        boneEntity->addComponent(sphereTransform);
*/

        a->spheres.push_back(sphereEntity);
        a->transforms.push_back(sphereTransform);
//        a->bones.push_back(boneEntity);
    }

    m_animations.push_back(a);
    drawUpdate();
    checkSliderRange();
}

void AnimPlayer::removeAnimation(const MocapAnimation *anim)
{
    for (int i = 0; i < m_animations.size(); ++i)
    {
        if (m_animations[i]->animation == anim)
        {
            delete m_animations[i];
            m_animations.remove(i);
            break;
        }
    }
}

void AnimPlayer::sliderFrameChanged(int time)
{
    m_time = time;
    drawUpdate();
}

void AnimPlayer::drawUpdate()
{
    for (int i = 0; i < m_animations.size(); ++i)
    {
        m_animations[i]->update(m_time);
    }
}

void AnimPlayer::checkSliderRange()
{
    int maxRange = 1;

    for (int i = 0; i < m_animations.size(); ++i)
    {
        int frames = m_animations[i]->animation->frames();

        if (frames > maxRange)
        {
            maxRange = frames;
        }
    }

    ui->slider->setMaximum(maxRange);
}

void AnimPlayer::setupCamera()
{
    Qt3DRender::QCamera *camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 5.0f, 50.0f));
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
}

void MocapAnimation3D::update(int time)
{
    if (time >= animation->frames())
    {
        return;
    }

    for (int i = 0; i < transforms.size(); ++i)
    {
        cv::Vec3f v = animation->operator()(i,time);
        transforms[i]->setTranslation({v[0],v[1],v[2]});
    }
}
