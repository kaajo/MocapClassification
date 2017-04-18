#ifndef ANIMPLAYER_H
#define ANIMPLAYER_H

#include <QWidget>

#include <mocapanimation.h>

#include <QTimer>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QEntity>

namespace Ui {
class AnimPlayer;
}

struct MocapAnimation3D
{
    ~MocapAnimation3D()
    {
        qDeleteAll(spheres);
    }

    void update(int time)
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

    MocapAnimation* animation;
    QVector<Qt3DCore::QEntity*> spheres;
    QVector<Qt3DCore::QTransform*> transforms;
};

class AnimPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit AnimPlayer(QWidget *parent = 0);
    ~AnimPlayer();

    void addAnimation(MocapAnimation *anim);
    void removeAnimation(const MocapAnimation *anim);

signals:
    void timeChanged();

private slots:
    void sliderFrameChanged(int time);

private:
    Ui::AnimPlayer *ui;

    void drawUpdate();
    void checkSliderRange();

    int m_time = 0;
    QTimer *m_timer = nullptr;

    Qt3DExtras::Qt3DWindow* view = nullptr;

    Qt3DCore::QEntity *rootEntity;
    void setupCamera();

    QVector<MocapAnimation3D*> m_animations;
};

#endif // ANIMPLAYER_H
