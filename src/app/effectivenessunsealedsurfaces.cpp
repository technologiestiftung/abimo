#include "effectivenessunsealedsurfaces.h"
#include "pdr.h"

// parameter values x1, x2, x3, x4 and x5 (one column each)
// for calculating the effectiveness parameter n for unsealed surfaces
const float EffectivenessUnsealedSurfaces::EKA[]= {
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

EffectivenessUnsealedSurfaces::EffectivenessUnsealedSurfaces()
{

}

/**
 =======================================================================================================================
    g e t N U V ( Pointer auf aktuellen DataRecord) Berechnung eines Records (abgespecktes Raster)
 =======================================================================================================================
 */
float EffectivenessUnsealedSurfaces::getNUV(PDR &record)
{
    int K;
    float G020, BAG0;

    G020 = getG02((int) (record.nFK + 0.5));

    switch (record.NUT)
    {
    case 'W':
        if (G020 <= 10.0)
            BAG0 = 3.0F;
        else if (G020 <= 25.0)
            BAG0 = 4.0F;
        else
            BAG0 = 8.0F;
        break;

    default:
        K = (int) (record.ERT / 5);
        if (record.ERT > 49) K = (int) (record.ERT / 10 + 5);
        if (K <= 0) K = 1;
        if (K >= 4) K = K - 1;
        if (K > 13) K = 13;
        K = 5 * K - 2;
        BAG0 = EKA[K - 1] + EKA[K] * G020 + EKA[K + 1] * G020 * G020;
        if ((BAG0 >= 2.0) && (record.ERT < 60)) BAG0 = EKA[K - 3] * G020 + EKA[K - 2];
        if ((G020 >= 20.0) && (record.ERT >= 60)) BAG0 = EKA[K - 3] * G020 + EKA[K - 2];

        if (record.BER > 0 && (record.P1S == 0 && record.ETPS == 0)) /* Modifikation, wenn keine Sommerwerte */
            BAG0 = BAG0 * (0.9985F + 0.00284F * record.BER - 0.00000379762F * record.BER * record.BER);
        break;
    }

    return BAG0;
}

float EffectivenessUnsealedSurfaces::getG02(int nFK)
{
    const float G02tab [] = {
        0.0F,   0.0F,  0.0F,  0.0F,  0.3F,  0.8F,  1.4F,  2.4F,  3.7F,  5.0F,
        6.3F,   7.7F,  9.3F, 11.0F, 12.4F, 14.7F, 17.4F, 21.0F, 26.0F, 32.0F,
        39.4F, 44.7F, 48.0F, 50.7F, 52.7F, 54.0F, 55.0F, 55.0F, 55.0F, 55.0F, 55.0F
    };

    return G02tab[nFK];
}
