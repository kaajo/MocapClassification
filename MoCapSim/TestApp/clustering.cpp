#include "clustering.h"

#include <QDir>

#include <QDebug>

Clustering::Clustering(const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList matList = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for(const QString &string : matList)
    {
        cv::Mat mat = creator.loadMatrix(dirPath, string);

        if (mat.empty())
        {
            qDebug() << "ignoring file: " << string;
        }
        else if (mat.rows == 1 && mat.cols != 1)
        {
            m_metricMat.push_back(mat);
        }
        else if (mat.rows > 1 && (mat.rows == mat.cols))
        {
            m_similarityMat.push_back(mat);
        }
    }
}
