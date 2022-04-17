#ifndef EFFECTIVENESSUNSEALEDSURFACES_H
#define EFFECTIVENESSUNSEALEDSURFACES_H

#include "pdr.h"

class EffectivenessUnsealedSurfaces
{
private:
    const static float EKA[];
    static float getG02(int nFK);
    static float bag0_forest(float G020);
    static float bag0_default(float G020, int yield, int irrigation, bool notSummer);
    static float nonSummerCorrected(float x, int irrigation);

public:
    EffectivenessUnsealedSurfaces();
    static float getNUV(PDR &record);
};

#endif // EFFECTIVENESSUNSEALEDSURFACES_H
