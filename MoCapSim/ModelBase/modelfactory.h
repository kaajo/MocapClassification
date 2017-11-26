#ifndef MODELBASE_H
#define MODELBASE_H

#include <QString>

class MocapAnimation;

class ModelFactory
{

public:
    ModelFactory();

    QVector<MocapAnimation*> load(const QString &path, int maxNumber = -1);
    void save(const QString &path, const QVector<MocapAnimation*> anims);

private:
    QVector<QVector3D> parseCoordsLine(const QString &line);
};

#endif // MODELBASE_H
