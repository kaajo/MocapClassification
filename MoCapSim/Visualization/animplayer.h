#ifndef ANIMPLAYER_H
#define ANIMPLAYER_H

#include <QWidget>
#include <QTimer>

#include <mocapanimation.h>

namespace Qt3DCore {
class QEntity;
class QTransform;
}

namespace Qt3DExtras {
class Qt3DWindow;
}

namespace Ui {
class AnimPlayer;
}

struct MocapAnimation3D
{
    ~MocapAnimation3D()
    {
        qDeleteAll(spheres);
        qDeleteAll(bones);
    }

    void update(int time);

    MocapAnimation* animation;
    QVector<Qt3DCore::QEntity*> spheres;
    QVector<Qt3DCore::QTransform*> transforms;
    QVector<Qt3DCore::QEntity*> bones;
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
