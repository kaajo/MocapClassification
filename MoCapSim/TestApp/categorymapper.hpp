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

#ifndef CATEGORYMAPPER_HPP
#define CATEGORYMAPPER_HPP

#include <QVector>
#include <QHash>

class MocapAnimation;

class CategoryMapper
{
public:
    CategoryMapper();

    static void transform130to65(QVector<MocapAnimation*> anims);

    static QHash<int16_t,int16_t> trans130to65Pair;


};

#endif // CATEGORYMAPPER_HPP
