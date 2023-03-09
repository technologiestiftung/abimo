#include <QString>

#include "abimoreader.h"
#include "dbaseReader.h"
#include "helpers.h"

AbimoReader::AbimoReader(const QString& file) : DbaseReader(file)
{
    // Child constructor code here
}

QStringList AbimoReader::requiredFields()
{
    // The conversion function used in calculation.cpp to convert string to
    // numeric is given as a comment

    return {
        "NUTZUNG",    // toInt()
        "CODE",       // none?
        "REGENJA",    // toInt()
        "REGENSO",    // toInt()
        "FLUR",       // toFloat()
        "TYP",        // toInt()
        "FELD_30",    // toInt()
        "FELD_150",   // toInt()
        "BEZIRK",     // toInt()
        "PROBAU",     // toFloat()
        "PROVGU",     // toFloat()
        "VGSTRASSE",  // toFloat()
        "KAN_BEB",    // toFloat()
        "KAN_VGU",    // toFloat()
        "KAN_STR",    // toFloat()
        "BELAG1",     // toFloat()
        "BELAG2",     // toFloat()
        "BELAG3",     // toFloat()
        "BELAG4",     // toFloat()
        "STR_BELAG1", // toFloat()
        "STR_BELAG2", // toFloat()
        "STR_BELAG3", // toFloat()
        "STR_BELAG4", // toFloat()
        "FLGES",      // toFloat()
        "STR_FLGES"   // toFloat()
      };
}

bool AbimoReader::checkAndRead()
{
    QString fileName = m_file.fileName();
    QString text;

    bool succeeded = DbaseReader::checkAndRead();

    if (!succeeded) {
        return false;
    }

    if (!isAbimoFile()) {
        text = "Die Datei '%1' ist kein valider 'Input File',\n";
        text += "Ueberpruefen Sie die Spaltennamen und die Vollstaendigkeit.";
        m_fullError = text.arg(fileName);
        return false;
    }

    m_fullError = "";
    return true;
}

bool AbimoReader::isAbimoFile()
{
    return helpers::containsAll(m_hash, requiredFields());
}

void AbimoReader::fillRecord(int k, AbimoRecord& record, bool debug)
{
    record.unbuiltSealedFractionSurface[1] = floatFraction(getRecord(k, "BELAG1"));
    record.unbuiltSealedFractionSurface[2] = floatFraction(getRecord(k, "BELAG2"));
    record.unbuiltSealedFractionSurface[3] = floatFraction(getRecord(k, "BELAG3"));
    record.unbuiltSealedFractionSurface[4] = floatFraction(getRecord(k, "BELAG4"));
    record.district = getRecord(k, "BEZIRK").toInt();
    record.code = getRecord(k, "CODE");
    record.fieldCapacity_150 = getRecord(k, "FELD_150").toInt();
    record.fieldCapacity_30 = getRecord(k, "FELD_30").toInt();
    record.mainArea = getRecord(k, "FLGES").toFloat();
    record.depthToWaterTable = getRecord(k, "FLUR").toFloat();
    record.builtSealedFractionConnected = floatFraction(getRecord(k, "KAN_BEB"));
    record.roadSealedFractionConnected = floatFraction(getRecord(k, "KAN_STR"));
    record.unbuiltSealedFractionConnected = floatFraction(getRecord(k, "KAN_VGU"));
    record.usage = helpers::stringToInt(
        getRecord(k, "NUTZUNG"),
        QString("k: %1, NUTZUNG = ").arg(QString::number(k)),
        debug
    );
    record.mainFractionBuiltSealed = helpers::stringToFloat(
        getRecord(k, "PROBAU"),
        QString("k: %1, PROBAU = ").arg(QString::number(k)),
        debug
    ) / 100.0F;
    record.mainFractionUnbuiltSealed = floatFraction(getRecord(k, "PROVGU"));
    record.precipitationYear = getRecord(k, "REGENJA").toInt();
    record.precipitationSummer = getRecord(k, "REGENSO").toInt();
    record.roadSealedFractionSurface[1] = floatFraction(getRecord(k, "STR_BELAG1"));
    record.roadSealedFractionSurface[2] = floatFraction(getRecord(k, "STR_BELAG2"));
    record.roadSealedFractionSurface[3] = floatFraction(getRecord(k, "STR_BELAG3"));
    record.roadSealedFractionSurface[4] = floatFraction(getRecord(k, "STR_BELAG4"));
    record.type = getRecord(k, "TYP").toInt();
    record.roadFractionSealed = floatFraction(getRecord(k, "VGSTRASSE"));
    record.roadArea = getRecord(k, "STR_FLGES").toFloat();
}

float AbimoReader::floatFraction(QString string)
{
    return (string.toFloat() / 100.0);
}
