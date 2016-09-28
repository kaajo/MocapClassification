#ifndef MODELBASE_H
#define MODELBASE_H

#include "mocapanimation.h"

#include <QString>

class ModelFactory
{

public:
    ModelFactory();

    QVector<MocapAnimation *> load(const QString &path);

private:
    MocapAnimation::MocapPose parseCoordsLine(const QString &line);
};

#endif // MODELBASE_H
