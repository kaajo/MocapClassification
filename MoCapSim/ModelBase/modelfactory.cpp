#include "modelfactory.h"

#include <QFile>
#include <QTextStream>

ModelFactory::ModelFactory()
{
}

QVector<MocapAnimation*> ModelFactory::load(const QString &path)
{
    QVector<MocapAnimation*> retVal;

    QFile file(path);
    file.open(QFile::OpenModeFlag::ReadOnly);

    if(!file.isOpen())
    {
        return retVal;
    }

    QVector<MocapAnimation::MocapPose> currentPoses;
    int currCatID = -1;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line[0] == '#')
        {
            if (retVal.size() > 40)
            {
                //break;
            }

            if (currCatID != -1)
            {
                retVal.push_back(new MocapAnimation(currCatID,currentPoses));
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
            currentPoses.push_back(parseCoordsLine(line));
        }

    }

    if (! currentPoses.empty())
    {
        retVal.push_back(new MocapAnimation(currCatID,currentPoses));
    }


    return retVal;
}

MocapAnimation::MocapPose ModelFactory::parseCoordsLine(const QString &line)
{
    QStringList coordsList = line.split(";", QString::SkipEmptyParts);

    MocapAnimation::MocapPose retVal;

    for (int i = 0; i < coordsList.size(); ++i)
    {
        QStringList coords = coordsList[i].split(",", QString::SkipEmptyParts);

        retVal.push_back(QVector3D(coords[0].toFloat(), coords[1].toFloat(), coords[2].toFloat()));
    }

    return retVal;
}
