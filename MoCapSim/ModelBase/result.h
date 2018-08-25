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

#ifndef RESULT_H
#define RESULT_H

#include <QPair>
#include <QVector>

#include <iostream>

class MocapAnimation;

class Result
{
public:
    Result(){}
    Result(const MocapAnimation *anim,const QVector<QPair<float,MocapAnimation*>> res);

    void printResult();

    ///stats
    bool isCategoryMatched(const int noRes) const;
    bool isCategoryMatched() const;
    inline size_t noMatched(const int noRes);
    int firstMatched();

    ///getters/setters
    inline const MocapAnimation* animation() const {return m_animation;}
    inline const QVector<QPair<float, MocapAnimation*>>& distance() const {return m_distance;}

private:
    const MocapAnimation *m_animation = nullptr;
    QVector<QPair<float,MocapAnimation*>> m_distance;

};

#endif // RESULT_H
