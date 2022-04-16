#include "config.h"

Config::Config()
{
}

//=======================================================================================================================
//    Bestimmung der Durchwurzelungstiefe TWS
//=======================================================================================================================

float Config::getTWS(int ert, char nutz)
{
    // Zuordnung Durchwurzelungstiefe in Abhaengigkeit der Nutzung

    // D - Devastierung
    if (nutz == 'D') return 0.2F;

    // L - landwirtschaftliche Nutzung
    if (nutz == 'L') return (ert <= 50) ? 0.6F : 0.7F;

    // K - Kleingaerten
    if (nutz == 'K') return 0.7F;

    // W - Wald
    if (nutz == 'W') return 1.0F;

    // Other
    return 0.2F;
}
