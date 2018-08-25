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

#include "result.h"

#include <algorithm>

#include "mocapanimation.h"

Result::Result(const MocapAnimation *anim,const QVector<QPair<float, MocapAnimation *> > distance)
    : m_animation(anim),
      m_distance(distance)
{
    auto cmp = [](QPair<float,MocapAnimation*> l, QPair<float,MocapAnimation*> r){return l.first < r.first;};

    std::sort(m_distance.begin(),m_distance.end(),cmp);
}

void Result::printResult()
{
    if (isCategoryMatched())
    {
        //std::cout << "ID: " << m_animation->getId() << " MATCH!" << std::endl;
    }
    else
    {
        std::cout << "ID: " << m_animation->getId() << " FAIL: category: " << m_animation->getRealCategory() << " closest to: "
                  << " ID: " << m_distance.first().second->getId() << " category: " << m_distance.first().second->getRealCategory()
                  << " first matched " << firstMatched() << std::endl;
    }
}

bool Result::isCategoryMatched() const {return m_animation->getRealCategory() == m_distance.first().second->getRealCategory();}

bool Result::isCategoryMatched(const int noRes) const
{
    const int maxI = std::min(noRes,m_distance.size());

    for (int i = 0; i < maxI; ++i)
    {
        if(m_distance.at(i).second->getRealCategory() == m_animation->getRealCategory())
        {
            return true;
        }
    }
    return false;
}

size_t Result::noMatched(const int noRes)
{
    const int maxI = std::min(noRes,m_distance.size());

    size_t counter = 0;

    for (int i = 0; i < maxI; ++i)
    {
        if(m_distance.at(i).second->getRealCategory() == m_animation->getRealCategory())
        {
            ++counter;
        }
    }

    return counter;
}

int Result::firstMatched()
{
    for (int i = 0; i < m_distance.size(); ++i)
    {
         if(m_distance.at(i).second->getRealCategory() == m_animation->getRealCategory())
         {
             return i + 1;
         }
    }

    return -1;
}
