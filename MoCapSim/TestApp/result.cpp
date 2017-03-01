#include "result.h"

#include <algorithm>

Result::Result(MocapAnimation *anim, QVector<QPair<float, MocapAnimation *> > distance)
    : m_animiation(anim),m_distance(distance)
{
    auto cmp = [](QPair<float,MocapAnimation*> l, QPair<float,MocapAnimation*> r){return l.first < r.first;};

    std::sort(m_distance.begin(),m_distance.end(),cmp);
}
