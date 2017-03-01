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
    Result(MocapAnimation *anim, QVector<QPair<float,MocapAnimation*>> res);

    MocapAnimation *m_animiation = nullptr;
    QVector<QPair<float,MocapAnimation*>> m_distance;

    void printResult(const MocapAnimation *const anim, const Result &result)
    {
        if (anim->getRealCategory() == result.m_distance.first().second->getRealCategory())
        {
            std::cout << anim->m_id << " MATCH!" << std::endl;
        }
        else
        {
            std::cout << anim->m_id << " MISS:  first category: " << anim->getRealCategory() << " second: "
                      << result.m_distance.first().second->getRealCategory() << " id: " << result.m_distance.first().second->m_id << std::endl;
        }

    }

};

#endif // RESULT_H
