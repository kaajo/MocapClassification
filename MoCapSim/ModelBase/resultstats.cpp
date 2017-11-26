#include "mocapanimation.h"

#include "resultstats.hpp"

ResultMetrics::ResultMetrics(QVector<Result> &results) :
    m_results(results)
{

}

float ResultMetrics::getAccuracy(const int numOfResults) const
{
    int matchedCounter = 0;
    int animsCounter = 0;

    for (int i = 0; i < m_results.size(); ++i)
    {
        ++animsCounter;

        if (m_results[i].isCategoryMatched(numOfResults))
        {
            ++matchedCounter;
        }
    }

    return matchedCounter/static_cast<float>(animsCounter);
}

QMap<int16_t, qreal> ResultMetrics::getCategoryAccuracy(const int numOfResults) const
{
    QMap<int16_t, qreal> matchedCounters;
    QMap<int16_t, qreal> animCounters;

    for (int i = 0; i < m_results.size(); ++i)
    {
        int16_t cat = m_results[i].animation()->getRealCategory();

        ++animCounters[cat];

        if (m_results[i].isCategoryMatched(numOfResults))
        {
            ++matchedCounters[cat];
        }
    }

    for (int16_t val : animCounters.keys())
    {
        matchedCounters[val] /= animCounters[val];
    }

    return matchedCounters;
}
