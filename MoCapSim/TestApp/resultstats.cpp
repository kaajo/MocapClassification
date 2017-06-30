#include "resultstats.hpp"

QVector<int> ResultMetrics::m_skipCategories  = {56, 57,58,59,60, 61 , 138 , 139};

ResultMetrics::ResultMetrics(QVector<Result> &results) :
    m_results(results)
{

}

float ResultMetrics::getAccuracy(const int numOfResults)
{
    int matchedCounter = 0;
    int animsCounter = 0;

    for (int i = 0; i < m_results.size(); ++i)
    {
        if (m_skipCategories.contains(m_results[i].animation()->getRealCategory()))
        {
            continue;
        }

        ++animsCounter;

        if (m_results[i].isCategoryMatched(numOfResults))
        {
            ++matchedCounter;
        }
    }

    return matchedCounter/(float)animsCounter;
}

std::map<int16_t, float> ResultMetrics::getCategoryAccuracy(const int numOfResults)
{
    std::map<int16_t, float> matchedCounters;
    std::map<int16_t, float> animCounters;

    for (int i = 0; i < m_results.size(); ++i)
    {
        int16_t cat = m_results[i].animation()->getRealCategory();

        if (m_skipCategories.contains(cat))
        {
            continue;
        }

        ++animCounters[cat];

        if (m_results[i].isCategoryMatched(numOfResults))
        {
            ++matchedCounters[cat];
        }
    }

    for (auto &pair : matchedCounters)
    {
        pair.second /= animCounters[pair.first];
    }

    return matchedCounters;
}
