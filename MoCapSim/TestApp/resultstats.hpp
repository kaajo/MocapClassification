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
    float getAccuracy(const int numOfResults = 1);

    /**
     * @brief getCategoryAccuracy
     * @return
     */
    std::map<int16_t,float> getCategoryAccuracy(const int numOfResults = 1);

private:
    QVector<Result> &m_results;

    static QVector<int> m_skipCategories;
};

#endif // RESULTMETRICS_HPP
