/***************************************************************************
 * For copyright information please see COPYRIGHT.txt in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#include <math.h> // for abs()

#include "config.h"
#include "constants.h" // for MIN() macro
#include "pdr.h"

PDR::PDR():
    wIndex(0),
    nFK(0),
    FLW(0),
    NUT(Usage::unknown),
    R(0),
    ROW(0),
    RI(0),
    VER(0),
    ERT(0),
    BER(0),
    P1(0.0F),
    ETP(0),
    KR(0),
    P1S(0.0F),
    ETPS(0)
{}

void PDR::setUsageYieldIrrigation(Usage usage, int yield, int irrigation)
{
    this->NUT = usage;
    this->ERT = yield;
    this->BER = irrigation;
}

void PDR::setUsageYieldIrrigation(UsageTuple tuple)
{
    this->NUT = tuple.usage;
    this->ERT = tuple.yield;
    this->BER = tuple.irrigation;
}

// mittlere Zahl der Wachstumstage
int PDR::estimateDaysOfGrowth(Usage usage, int yield)
{
    switch (usage)
    {
        case Usage::agricultural_L: return (yield <= 50) ? 60 : 75;
        case Usage::horticultural_K: return 100;
        case Usage::forested_W: return 90;
        case Usage::vegetationless_D: return 50;
        default: return 50;
    }
}

float PDR::estimateWaterHoldingCapacity(int f30, int f150, bool isForest)
{
    if (MIN(f30, f150) < 1) {
        return 13.0F;
    }

    if (abs(f30 - f150) < MIN(f30, f150)) { // unwesentliche Abweichung
        return (float) (isForest ? f150 : f30);
    }

    return
        0.75F * (float) (isForest ? f150 : f30) +
        0.25F * (float) (isForest ? f30 : f150);
}
