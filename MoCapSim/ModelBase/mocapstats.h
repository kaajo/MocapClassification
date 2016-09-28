#ifndef MOCAPSTATS_H
#define MOCAPSTATS_H

#include "mocapanimation.h"

class MocapStats
{
public:
    MocapStats();

    static void getStats(std::vector<std::vector<MocapAnimation*>> categories);
};

#endif // MOCAPSTATS_H
