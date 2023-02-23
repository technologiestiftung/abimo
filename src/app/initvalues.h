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
    void setInfdach(float v);
    void setInfbel1(float v);
    void setInfbel2(float v);
    void setInfbel3(float v);
    void setInfbel4(float v);
    void setBagdach(float v);
    void setBagbel1(float v);
    void setBagbel2(float v);
    void setBagbel3(float v);
    void setBagbel4(float v);
    void setDecR(int v);
    void setDecROW(int v);
    void setDecRI(int v);
    void setDecRVOL(int v);
    void setDecROWVOL(int v);
    void setDecRIVOL(int v);
    void setDecFLAECHE(int v);
    void setDecVERDUNSTUNG(int v);
    void setBERtoZero(bool v);
    void setNiedKorrF(float v);
    float getInfdach();
    float getInfbel1();
    float getInfbel2();
    float getInfbel3();
    float getInfbel4();
    float getBagdach();
    float getBagbel1();
    float getBagbel2();
    float getBagbel3();
    float getBagbel4();
    int getDecR();
    int getDecROW();
    int getDecRI();
    int getDecRVOL();
    int getDecROWVOL();
    int getDecRIVOL();
    int getDecFLAECHE();
    int getDecVERDUNSTUNG();
    bool getBERtoZero();
    float getPrecipitationCorrectionFactor();
    bool allSet();
    void putToHash(QString bezirkeString, int value, int hashtyp);
    QHash<int, int> hashETP;
    QHash<int, int> hashETPS;
    QHash<int, int> hashEG;
    int getCountSets();

private:
    // Infiltrationsfaktoren
    float infdach, infbel1, infbel2, infbel3, infbel4;

    // Bagrovwerte
    float bagdach, bagbel1, bagbel2, bagbel3, bagbel4;

    // Nachkomma
    int decR, decROW, decRI, decRVOL, decROWVOL, decRIVOL, decFLAECHE, decVERDUNSTUNG;

    // BER to Zero hack
    bool BERtoZero;

    // Niederschlags-Korrekturfaktor
    float niedKorrF;

    int countSets;

    void putToHashL(QString bezirkeString, int value, QHash<int, int> &hash);
};

#endif
