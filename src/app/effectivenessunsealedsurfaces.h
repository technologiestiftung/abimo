#ifndef EFFECTIVENESSUNSEALEDSURFACES_H
#define EFFECTIVENESSUNSEALEDSURFACES_H

#include "pdr.h"

class EffectivenessUnsealedSurfaces
{
private:
    const static float EKA[];
    static float getG02(int nFK);

public:
    EffectivenessUnsealedSurfaces();
    static float getNUV(PDR &record);
};

#endif // EFFECTIVENESSUNSEALEDSURFACES_H
