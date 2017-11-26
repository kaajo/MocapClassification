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
