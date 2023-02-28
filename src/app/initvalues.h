/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef INITVALUES_H
#define INITVALUES_H

#include <QHash>
#include <QString>

class InitValues
{

public:
    InitValues();
    ~InitValues();
    static QString updateFromConfig(InitValues &initValues, QString configFileName);
    void setInfiltrationFactorRoof(float v);
    void setInfiltrationFactorSurfaceClass1(float v);
    void setInfiltrationFactorSurfaceClass2(float v);
    void setInfiltrationFactorSurfaceClass3(float v);
    void setInfiltrationFactorSurfaceClass4(float v);
    void setBagrovRoof(float v);
    void setBagrovSufaceClass1(float v);
    void setBagrovSufaceClass2(float v);
    void setBagrovSufaceClass3(float v);
    void setBagrovSufaceClass4(float v);
    void setDigitsTotalSystemLosses(int v);
    void setDigitsRunoff(int v);
    void setDigitsInfiltrationRate(int v);
    void setDigitsTotalRunoff(int v);
    void setDigitsRainwaterRunoff(int v);
    void setDigitsTotalSubsurfaceFlow(int v);
    void setDigitsTotalArea(int v);
    void setDigitsEvaporation(int v);
    void setIrrigationToZero(bool v);
    void setPrecipitationCorrectionFactor(float v);
    float getInfiltrationFactorRoof();
    float getInfiltrationFactorSurfaceClass1();
    float getInfiltrationFactorSurfaceClass2();
    float getInfiltrationFactorSurfaceClass3();
    float getInfiltrationFactorSurfaceClass4();
    float getBagrovRoof();
    float getBagrovSufaceClass1();
    float getBagrovSufaceClass2();
    float getBagrovSufaceClass3();
    float getBagrovSufaceClass4();
    int getDigitsTotalSystemLosses();
    int getDigitsRunoff();
    int getDigitsInfiltrationRate();
    int getDigitsTotalRunoff();
    int getDigitsRainwaterRunoff();
    int getDigitsTotalSubsurfaceFlow();
    int getDigitsTotalArea();
    int getDigitsEvaporation();
    bool getIrrigationToZero();
    float getPrecipitationCorrectionFactor();
    bool allSet();
    void putToHashOfType(QString districts, int value, int hashType);
    QHash<int, int> hashETP;
    QHash<int, int> hashETPS;
    QHash<int, int> hashEG;
    int getCountSets();

private:
    // Infiltrationsfaktoren
    float infiltrationFactorRoof; // old: infdach
    float infiltrationFactorSurfaceClass1; // old: infbel1
    float infiltrationFactorSurfaceClass2; // old: infbel2
    float infiltrationFactorSurfaceClass3; // old: infbel3
    float infiltrationFactorSurfaceClass4; // old: infbel4

    // Bagrovwerte
    float bagrovRoof; // old: bagdach
    float bagrovSufaceClass1; // old: bagbel1
    float bagrovSufaceClass2; // old: bagbel2
    float bagrovSufaceClass3; // old: bagbel3
    float bagrovSufaceClass4; // old: bagbel4

    // Nachkomma
    int digitsTotalSystemLosses; // old: decR
    int digitsRunoff; // old: decROW
    int digitsInfiltrationRate; // old: decRI
    int digitsTotalRunoff; // old: decRVOL
    int digitsRainwaterRunoff; // old: decROWVOL
    int digitsTotalSubsurfaceFlow; // old: decRIVOL
    int digitsTotalArea; // old: decFLAECHE
    int digitsEvaporation; // old: decVERDUNSTUNG

    // BER to Zero hack
    bool irrigationToZero; // old: BERtoZero

    // Niederschlags-Korrekturfaktor
    float precipitationCorrectionFactor; // old: niedKorrF

    int countSets;

    void putToReferencedHash(QString districts, int value, QHash<int, int> &hash);
};

#endif
