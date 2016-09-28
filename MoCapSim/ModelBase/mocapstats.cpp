#include "mocapstats.h"

#include <QDebug>

MocapStats::MocapStats()
{

}

void MocapStats::getStats(std::vector<std::vector<MocapAnimation *> > categories)
{
    qDebug() << "processing stats for " << categories.size() << " categories";

    for(int i = 0; i < categories.size(); ++i)
    {
        for(int j = 0; j < categories[i].size(); ++j)
        {
            qDebug() << i << " " << categories[i][j]->getRealCategory();
        }
    }

}
