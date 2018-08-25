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
};

#endif // RESULTMETRICS_HPP
