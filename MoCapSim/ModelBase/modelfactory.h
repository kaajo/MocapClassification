#ifndef MODELBASE_H
#define MODELBASE_H

#include "mocapanimation.h"

#include <QString>

class ModelFactory
{

public:
    ModelFactory();

    QVector<MocapAnimation*> load(const QString &path, int maxNumber = -1);
    void save(const QString &path, const QVector<MocapAnimation*> anims);

private:
    MocapAnimation::MocapFrame parseCoordsLine(const QString &line);
};

#endif // MODELBASE_H
