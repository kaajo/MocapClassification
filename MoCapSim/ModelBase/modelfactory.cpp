/*
    Copyright (C) 2017  Miroslav Krajíček

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
