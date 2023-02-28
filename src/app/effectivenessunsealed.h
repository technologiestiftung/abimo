#ifndef EFFECTIVENESSUNSEALED_H
#define EFFECTIVENESSUNSEALED_H

#include "pdr.h"

class EffectivenessUnsealed
{
private:
    const static float EKA[];
    static float getG02(int nFK);
    static float bag0_forest(float G020);
    static float bag0_default(float G020, int yield, int irrigation, bool notSummer);
    static float nonSummerCorrected(float x, int irrigation);

public:
    EffectivenessUnsealed();
    static float calculate(PDR &record);
};

#endif // EFFECTIVENESSUNSEALED_H
