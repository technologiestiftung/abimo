/***************************************************************************
 * For copyright information please see COPYRIGHT.txt in the base directory
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
    infdach(0.00F),
    infbel1(0.10F),
    infbel2(0.30F),
    infbel3(0.60F),
    infbel4(0.90F),

    /* Bagrovwerte */
    bagdach(0.05F),
    bagbel1(0.11F),
    bagbel2(0.11F),
    bagbel3(0.25F),
    bagbel4(0.40F),

    /* Nachkomma der Ergebnisse */
    decR(3),
    decROW(3),
    decRI(3),
    decRVOL(3),
    decROWVOL(3),
    decRIVOL(3),
    decFLAECHE(0),
    decVERDUNSTUNG(3),

    BERtoZero(false),
    niedKorrF(1.09f),
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

void InitValues::setInfdach(float v) {
    infdach = v;
    countSets |= 1;
}

void InitValues::setInfbel1(float v) {
    infbel1 = v;
    countSets |= 2;
}

void InitValues::setInfbel2(float v) {
    infbel2 = v;
    countSets |= 4;
}

void InitValues::setInfbel3(float v) {
    infbel3 = v;
    countSets |= 8;
}

void InitValues::setInfbel4(float v) {
    infbel4 = v;
    countSets |= 16;
}

void InitValues::setBagdach(float v) {
    bagdach = v;
    countSets |= 32;
}

void InitValues::setBagbel1(float v) {
    bagbel1 = v;
    countSets |= 64;
}

void InitValues::setBagbel2(float v) {
    bagbel2 = v;
    countSets |= 128;
}

void InitValues::setBagbel3(float v) {
    bagbel3 = v;
    countSets |= 256;
}

void InitValues::setBagbel4(float v) {
    bagbel4 = v;
    countSets |= 512;
}

void InitValues::setDecR(int v) {
    decR = v;
    countSets |= 1024;
}

void InitValues::setDecROW(int v) {
    decROW = v;
    countSets |= 2048;
}

void InitValues::setDecRI(int v) {
    decRI = v;
    countSets |= 4096;
}

void InitValues::setDecRVOL(int v) {
    decRVOL = v;
    countSets |= 8192;
}

void InitValues::setDecROWVOL(int v) {
    decROWVOL = v;
    countSets |= 16384;
}

void InitValues::setDecRIVOL(int v) {
    decRIVOL = v;
    countSets |= 32768;
}

void InitValues::setDecFLAECHE(int v) {
    decFLAECHE = v;
    countSets |= 65536;
}

void InitValues::setDecVERDUNSTUNG(int v) {
    decVERDUNSTUNG = v;
    countSets |= 131072;
}

void InitValues::setBERtoZero(bool v) {
    BERtoZero = v;
    countSets |= 262144;
}

void InitValues::setNiedKorrF(float v) {
    niedKorrF = v;
    countSets |= 524288;
}

float InitValues::getInfdach()
{
    return infdach;
}

float InitValues::getInfbel1() {
    return infbel1;
}

float InitValues::getInfbel2() {
    return infbel2;
}

float InitValues::getInfbel3() {
    return infbel3;
}

float InitValues::getInfbel4() {
    return infbel4;
}

float InitValues::getBagdach() {
    return bagdach;
}

float InitValues::getBagbel1() {
    return bagbel1;
}

float InitValues::getBagbel2() {
    return bagbel2;
}

float InitValues::getBagbel3() {
    return bagbel3;
}

float InitValues::getBagbel4() {
    return bagbel4;
}

int InitValues::getDecR() {
    return decR;
}

int InitValues::getDecROW() {
    return decROW;
}

int InitValues::getDecRI() {
    return decRI;
}

int InitValues::getDecRVOL() {
    return decRVOL;
}

int InitValues::getDecROWVOL() {
    return decROWVOL;
}

int InitValues::getDecRIVOL() {
    return decRIVOL;
}

int InitValues::getDecFLAECHE() {
    return decFLAECHE;
}

int InitValues::getDecVERDUNSTUNG() {
    return decVERDUNSTUNG;
}

bool InitValues::getBERtoZero() {
    return BERtoZero;
}

float InitValues::getNiedKorrF() {
    return niedKorrF;
}

bool InitValues::allSet() {
    return countSets == 1048575;
}

int InitValues::getCountSets() {
    return countSets;
}

void InitValues::putToHash(QString bezirkeString, int value, int hashtyp) {

    if (hashtyp == 11) {
        putToHashL(bezirkeString, value, hashETP);
    }
    else if (hashtyp == 12) {
        putToHashL(bezirkeString, value, hashETPS);
    }
    else if (hashtyp == 13) {
        putToHashL(bezirkeString, value, hashEG);
    }
}

void InitValues::putToHashL(QString bezirkeString, int value, QHash<int, int> &hash) {

    if (bezirkeString.length() == 0) {
        bezirkeString = "0";
    }

    QStringList bezlist = bezirkeString.split(",", QString::SkipEmptyParts);

    for (int i = 0; i < bezlist.size(); ++i) {
        QString bezBereich = bezlist.at(i).trimmed();
        if (bezBereich.contains("-")) {
            QStringList bezBereichlist = bezBereich.split("-", QString::SkipEmptyParts);
            int min =   bezBereichlist.at(0).trimmed().toInt();
            int max =   bezBereichlist.at(1).trimmed().toInt();
            for (int bezirk=min; bezirk <= max; ++bezirk) {
                hash[bezirk] = value;
            }
        }
        else {
            hash[bezBereich.toInt()] = value;
        }
    }
}
