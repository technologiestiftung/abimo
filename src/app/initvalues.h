/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef INITVALUES_H
#define INITVALUES_H

#include <array>

#include <QHash>
#include <QString>

class InitValues
{

public:
    InitValues();
    static QString updateFromConfig(InitValues &initValues, QString configFileName);
    void setInfiltrationFactor(int index, float v);
    void setBagrovValue(int index, float value);
    void setDigitsTotalRunoff(int v);
    void setDigitsRunoff(int v);
    void setDigitsInfiltrationRate(int v);
    void setDigitsTotalRunoffFlow(int v);
    void setDigitsRainwaterRunoff(int v);
    void setDigitsTotalSubsurfaceFlow(int v);
    void setDigitsTotalArea(int v);
    void setDigitsEvaporation(int v);
    void setIrrigationToZero(bool v);
    void setPrecipitationCorrectionFactor(float v);
    float getInfiltrationFactor(int index);
    float getBagrovValue(int index);
    int getDigitsTotalRunoff();
    int getDigitsRunoff();
    int getDigitsInfiltrationRate();
    int getDigitsTotalRunoffFlow();
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
    // index 0: roof, indices 1-4: surface classes 1-4
    std::array<float,5> infiltrationFactors; // old: infdach, infbel1 - infbel4

    // Bagrovwerte
    // index 0: roof, indices 1-4: surface classes 1-4
    std::array<float,5> bagrovValues;

    // Nachkomma
    int digitsTotalRunoff; // old: decR
    int digitsRunoff; // old: decROW
    int digitsInfiltrationRate; // old: decRI
    int digitsTotalRunoffFlow; // old: decRVOL
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
