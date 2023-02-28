#include "effectivenessunsealed.h"

#include "constants.h"
#include "pdr.h"

// parameter values x1, x2, x3, x4 and x5 (one column each)
// for calculating the effectiveness parameter n for unsealed surfaces
const float EffectivenessUnsealed::EKA[]= {
    0.04176F, -0.647F , 0.218F  ,  0.01472F, 0.0002089F,
    0.04594F, -0.314F , 0.417F  ,  0.02463F, 0.0001143F,
    0.05177F, -0.010F , 0.596F  ,  0.02656F, 0.0002786F,
    0.05693F,  0.033F , 0.676F  ,  0.0279F , 0.00035F  ,
    0.06162F,  0.176F , 0.773F  ,  0.02809F, 0.0004695F,
    0.06962F,  0.24F  , 0.904F  ,  0.02562F, 0.0007149F,
    0.0796F ,  0.31F  , 1.039F  ,  0.0288F , 0.0008696F,
    0.07998F,  0.7603F, 1.2F    ,  0.0471F , 0.000293F ,
    0.08762F,  1.019F , 1.373F  ,  0.04099F, 0.0014141F,
    0.11833F,  1.1334F, 1.95F   ,  0.0525F , 0.00125F  ,
    0.155F  ,  1.5F   , 2.64999F,  0.0725F , 0.001249F ,
    0.20041F,  2.0918F, 3.69999F,  0.08F   , 0.001999F ,
    0.33895F,  3.721F , 6.69999F, -0.07F   , 0.013F
};

EffectivenessUnsealed::EffectivenessUnsealed()
{
}

/**
 =======================================================================================================================
    g e t N U V ( Pointer auf aktuellen DataRecord) Berechnung eines Records (abgespecktes Raster)
 =======================================================================================================================
 */
float EffectivenessUnsealed::calculate(PDR &record)
{
    float G020 = getG02((int) (record.nFK + 0.5));
    Usage usage = record.NUT;

    if (usage == Usage::forested_W) {
        return bag0_forest(G020);
    }

    return bag0_default(G020, record.ERT, record.BER, (record.P1S == 0 && record.ETPS == 0));
}

float EffectivenessUnsealed::getG02(int nFK)
{
    const float G02tab [] = {
        0.0F,   0.0F,  0.0F,  0.0F,  0.3F,  0.8F,  1.4F,  2.4F,  3.7F,  5.0F,
        6.3F,   7.7F,  9.3F, 11.0F, 12.4F, 14.7F, 17.4F, 21.0F, 26.0F, 32.0F,
        39.4F, 44.7F, 48.0F, 50.7F, 52.7F, 54.0F, 55.0F, 55.0F, 55.0F, 55.0F, 55.0F
    };

    return G02tab[nFK];
}

float EffectivenessUnsealed::bag0_forest(float G020)
{
    if (G020 <= 10.0) {
        return 3.0F;
    }

    if (G020 <= 25.0) {
        return 4.0F;
    }

    return 8.0F;
}

float EffectivenessUnsealed::bag0_default(float G020, int yield, int irrigation, bool notSummer)
{
    int k;
    float result;
    bool condition_1, condition_2;

    k = (int) ((yield > 49) ? (yield / 10 + 5) : (yield / 5));

    if (k <= 0) {
        k = 1;
    }

    if (k >= 4) {
        k--;
    }

    k = 5 * MIN(k, 13) - 2;

    result = EKA[k - 1] + EKA[k] * G020 + EKA[k + 1] * G020 * G020;

    condition_1 = (result >= 2.0) && (yield < 60);
    condition_2 = (G020 >= 20.0) && (yield >= 60);

    if (condition_1 || condition_2) {
        result = EKA[k - 3] * G020 + EKA[k - 2];
    }

    // Modifikation, wenn keine Sommerwerte
    if (irrigation > 0 && notSummer) {
        result = nonSummerCorrected(result, irrigation);
    }

    return result;
}

float EffectivenessUnsealed::nonSummerCorrected(float x, int irrigation)
{
    return x * (0.9985F + 0.00284F * irrigation - 0.00000379762F * irrigation * irrigation);
}
