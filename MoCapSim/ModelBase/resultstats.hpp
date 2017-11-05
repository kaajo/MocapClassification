#ifndef RESULTMETRICS_HPP
#define RESULTMETRICS_HPP

#include "result.h"

class ResultMetrics
{
public:
    ResultMetrics(QVector<Result> &results);

    /**
     * @brief getAccuracy
     * @param numOfResults
     * @return
     */
    float getAccuracy(const int numOfResults = 1) const;

    /**
     * @brief getCategoryAccuracy
     * @return
     */
    QMap<int16_t, qreal> getCategoryAccuracy(const int numOfResults = 1) const;

private:
    QVector<Result> &m_results;

    QVector<int> m_skipCategories = {56, 57,58,59,60, 61 , 138 , 139};
    //QVector<int> m_skipCategories;
};

#endif // RESULTMETRICS_HPP
