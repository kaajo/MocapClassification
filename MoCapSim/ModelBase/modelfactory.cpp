

#include <QFile>
#include <QTextStream>

#include <QLoggingCategory>

#include "modelfactory.h"
#include "mocapanimation.h"

ModelFactory::ModelFactory()
{
}

QVector<MocapAnimation*> ModelFactory::load(const QString &path, int maxNumber)
{
    QVector<MocapAnimation*> retVal;

    QFile file(path);
    file.open(QFile::OpenModeFlag::ReadOnly);

    if(!file.isOpen())
    {
        qWarning() << "could not load animations: " << file.errorString();
        return retVal;
    }

    QVector<MocapAnimation::MocapFrame> currentPoses;
    int currCatID = -1;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line[0] == '#')
        {
            if (currCatID != -1)
            {
                int id = retVal.size();
                retVal.push_back(new MocapAnimation(id,currCatID,currentPoses));
                currentPoses.clear();
            }

            QStringList parts = line.split(" ",QString::SkipEmptyParts);
            QStringList info = parts[2].split("_",QString::SkipEmptyParts);
            currCatID = info[1].toInt();

            QString secondLine = in.readLine();
            parts = line.split(";", QString::SkipEmptyParts);
        }
        else
        {
            QVector<QVector3D> frame = parseCoordsLine(line);

            if (frame.size() > 1)
            {
                currentPoses.push_back(frame);
            }
        }

        if (maxNumber != -1 && retVal.size() >= maxNumber)
        {
            break;
        }
    }

    if (! currentPoses.empty())
    {
        int id = retVal.size();
        retVal.push_back(new MocapAnimation(id,currCatID,currentPoses));
    }

    return retVal;
}

void ModelFactory::save(const QString &path, const QVector<MocapAnimation *> anims)
{
    //TODO
}

QVector<QVector3D> ModelFactory::parseCoordsLine(const QString &line)
{
    QStringList coordsList = line.split(";", QString::SkipEmptyParts);

    MocapAnimation::MocapFrame retVal;

    for (int i = 0; i < coordsList.size(); ++i)
    {
        QStringList coords = coordsList[i].split(",", QString::SkipEmptyParts);

        retVal.push_back(QVector3D(coords[0].toFloat(), coords[1].toFloat(), coords[2].toFloat()));
    }

    return retVal;
}
