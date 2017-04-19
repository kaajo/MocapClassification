#ifndef RESULT_H
#define RESULT_H

#include "mocapanimation.h"

#include <QPair>
#include <QVector>

#include <iostream>

class Result
{
public:
    Result(){}
    Result(const MocapAnimation *anim,const QVector<QPair<float,MocapAnimation*>> res);

    void printResult();

    ///stats
    bool isCategoryMatched() const {return m_animation->getRealCategory() == m_distance.first().second->getRealCategory();}
    bool isCategoryMatched(const int noRes) const;

    inline size_t noMatched(const int noRes);
    inline int firstMatched();

    ///getters/setters
    inline const MocapAnimation* animation() const {return m_animation;}
    inline const QVector<QPair<float, MocapAnimation*>>& distance() const {return m_distance;}

private:
    const MocapAnimation *m_animation = nullptr;
    QVector<QPair<float,MocapAnimation*>> m_distance;

};

#endif // RESULT_H
