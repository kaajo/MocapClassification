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
