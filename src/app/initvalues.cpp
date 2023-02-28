/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#include <QFile>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

#include "helpers.h"
#include "initvalues.h"
#include "saxhandler.h"

InitValues::InitValues():

    /* Infiltrationsfaktoren */
    infiltrationFactorRoof(0.00F),
    infiltrationFactorSurfaceClass1(0.10F),
    infiltrationFactorSurfaceClass2(0.30F),
    infiltrationFactorSurfaceClass3(0.60F),
    infiltrationFactorSurfaceClass4(0.90F),

    /* Bagrovwerte */
    bagrovRoof(0.05F),
    bagrovSufaceClass1(0.11F),
    bagrovSufaceClass2(0.11F),
    bagrovSufaceClass3(0.25F),
    bagrovSufaceClass4(0.40F),

    /* Nachkomma der Ergebnisse */
    digitsTotalSystemLosses(3),
    digitsRunoff(3),
    digitsInfiltrationRate(3),
    digitsTotalRunoff(3),
    digitsRainwaterRunoff(3),
    digitsTotalSubsurfaceFlow(3),
    digitsTotalArea(0),
    digitsEvaporation(3),

    irrigationToZero(false),
    precipitationCorrectionFactor(1.09f),
    countSets(0)
{
}

InitValues::~InitValues()
{
}

// Update initial values with values read from config.xml. Returns error message
QString InitValues::updateFromConfig(InitValues &initValues, QString configFileName)
{
    QString prefix = Helpers::singleQuote(configFileName) + ": ";

    QFile initFile(configFileName);

    if (! initFile.exists()) {
        return "Keine " + prefix + "gefunden.\nNutze Standardwerte.";
    }

    QXmlSimpleReader xmlReader;
    QXmlInputSource data(&initFile);

    SaxHandler handler(initValues);

    xmlReader.setContentHandler(&handler);
    xmlReader.setErrorHandler(&handler);

    // Empty error message (means success)
    QString errorMessage = QString();

    if (!xmlReader.parse(&data)) {
        errorMessage = prefix + "korrupte Datei.\n" + "Nutze Standardwerte.";
    }
    else if (! initValues.allSet()) {
        errorMessage = prefix + "fehlende Werte.\n" + "Ergaenze mit Standardwerten.";
    }

    initFile.close();

    return errorMessage;
}

void InitValues::setInfiltrationFactorRoof(float v) {
    infiltrationFactorRoof = v;
    countSets |= 1;
}

void InitValues::setInfiltrationFactorSurfaceClass1(float v) {
    infiltrationFactorSurfaceClass1 = v;
    countSets |= 2;
}

void InitValues::setInfiltrationFactorSurfaceClass2(float v) {
    infiltrationFactorSurfaceClass2 = v;
    countSets |= 4;
}

void InitValues::setInfiltrationFactorSurfaceClass3(float v) {
    infiltrationFactorSurfaceClass3 = v;
    countSets |= 8;
}

void InitValues::setInfiltrationFactorSurfaceClass4(float v) {
    infiltrationFactorSurfaceClass4 = v;
    countSets |= 16;
}

void InitValues::setBagrovRoof(float v) {
    bagrovRoof = v;
    countSets |= 32;
}

void InitValues::setBagrovSufaceClass1(float v) {
    bagrovSufaceClass1 = v;
    countSets |= 64;
}

void InitValues::setBagrovSufaceClass2(float v) {
    bagrovSufaceClass2 = v;
    countSets |= 128;
}

void InitValues::setBagrovSufaceClass3(float v) {
    bagrovSufaceClass3 = v;
    countSets |= 256;
}

void InitValues::setBagrovSufaceClass4(float v) {
    bagrovSufaceClass4 = v;
    countSets |= 512;
}

void InitValues::setDigitsTotalSystemLosses(int v) {
    digitsTotalSystemLosses = v;
    countSets |= 1024;
}

void InitValues::setDigitsRunoff(int v) {
    digitsRunoff = v;
    countSets |= 2048;
}

void InitValues::setDigitsInfiltrationRate(int v) {
    digitsInfiltrationRate = v;
    countSets |= 4096;
}

void InitValues::setDigitsTotalRunoff(int v) {
    digitsTotalRunoff = v;
    countSets |= 8192;
}

void InitValues::setDigitsRainwaterRunoff(int v) {
    digitsRainwaterRunoff = v;
    countSets |= 16384;
}

void InitValues::setDigitsTotalSubsurfaceFlow(int v) {
    digitsTotalSubsurfaceFlow = v;
    countSets |= 32768;
}

void InitValues::setDigitsTotalArea(int v) {
    digitsTotalArea = v;
    countSets |= 65536;
}

void InitValues::setDigitsEvaporation(int v) {
    digitsEvaporation = v;
    countSets |= 131072;
}

void InitValues::setIrrigationToZero(bool v) {
    irrigationToZero = v;
    countSets |= 262144;
}

void InitValues::setPrecipitationCorrectionFactor(float v) {
    precipitationCorrectionFactor = v;
    countSets |= 524288;
}

float InitValues::getInfiltrationFactorRoof()
{
    return infiltrationFactorRoof;
}

float InitValues::getInfiltrationFactorSurfaceClass1() {
    return infiltrationFactorSurfaceClass1;
}

float InitValues::getInfiltrationFactorSurfaceClass2() {
    return infiltrationFactorSurfaceClass2;
}

float InitValues::getInfiltrationFactorSurfaceClass3() {
    return infiltrationFactorSurfaceClass3;
}

float InitValues::getInfiltrationFactorSurfaceClass4() {
    return infiltrationFactorSurfaceClass4;
}

float InitValues::getBagrovRoof() {
    return bagrovRoof;
}

float InitValues::getBagrovSufaceClass1() {
    return bagrovSufaceClass1;
}

float InitValues::getBagrovSufaceClass2() {
    return bagrovSufaceClass2;
}

float InitValues::getBagrovSufaceClass3() {
    return bagrovSufaceClass3;
}

float InitValues::getBagrovSufaceClass4() {
    return bagrovSufaceClass4;
}

int InitValues::getDigitsTotalSystemLosses() {
    return digitsTotalSystemLosses;
}

int InitValues::getDigitsRunoff() {
    return digitsRunoff;
}

int InitValues::getDigitsInfiltrationRate() {
    return digitsInfiltrationRate;
}

int InitValues::getDigitsTotalRunoff() {
    return digitsTotalRunoff;
}

int InitValues::getDigitsRainwaterRunoff() {
    return digitsRainwaterRunoff;
}

int InitValues::getDigitsTotalSubsurfaceFlow() {
    return digitsTotalSubsurfaceFlow;
}

int InitValues::getDigitsTotalArea() {
    return digitsTotalArea;
}

int InitValues::getDigitsEvaporation() {
    return digitsEvaporation;
}

bool InitValues::getIrrigationToZero() {
    return irrigationToZero;
}

float InitValues::getPrecipitationCorrectionFactor() {
    return precipitationCorrectionFactor;
}

bool InitValues::allSet() {
    return countSets == 1048575;
}

int InitValues::getCountSets() {
    return countSets;
}

void InitValues::putToHash(QString districts, int value, int hashType) {

    if (hashType == 11) {
        putToHashL(districts, value, hashETP);
    }
    else if (hashType == 12) {
        putToHashL(districts, value, hashETPS);
    }
    else if (hashType == 13) {
        putToHashL(districts, value, hashEG);
    }
}

void InitValues::putToHashL(QString districts, int value, QHash<int, int> &hash) {

    if (districts.length() == 0) {
        districts = "0";
    }

    QStringList districtList = districts.split(",", QString::SkipEmptyParts);

    for (int i = 0; i < districtList.size(); ++i) {
        QString districtRange = districtList.at(i).trimmed();
        if (districtRange.contains("-")) {
            QStringList rangeLimits = districtRange.split("-", QString::SkipEmptyParts);
            int minId = rangeLimits.at(0).trimmed().toInt();
            int maxId = rangeLimits.at(1).trimmed().toInt();
            for (int id = minId; id <= maxId; ++id) {
                hash[id] = value;
            }
        }
        else {
            hash[districtRange.toInt()] = value;
        }
    }
}
