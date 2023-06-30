/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#include <QDebug>

#include "saxHandler.h"

SaxHandler::SaxHandler(InitValues &initValues):
    state(ParameterGroup::None),
    initValues(initValues)
{}

bool SaxHandler::startElement(
    const QString &,
    const QString &,
    const QString &qName,
    const QXmlAttributes &attribs
)
{
    if (qName == "section") {
        state = nameToState(attribs.value("name"));
        return (state != ParameterGroup::None && state != ParameterGroup::Invalid);
    }

    if (qName == "item") {

        QString key = attribs.value("key");
        QString value = attribs.value("value");

        switch (state) {

        case ParameterGroup::Infiltrationsfaktoren:
            setInfiltrationFactor(key, value.toFloat());
            break;

        case ParameterGroup::Bagrovwerte :
            setBagrovValue(key, value.toFloat());
            break;

        case ParameterGroup::Nachkomma :
            setDigits(key, value.toInt());
            break;

        case ParameterGroup::Diverse :
            setDiverse(key, value);
            break;

        case ParameterGroup::GewVerd :
            waterEvaporationEntry(attribs);
            break;

        case ParameterGroup::PotVerd :
            potentialEvaporationEntry(attribs);
            break;

        case ParameterGroup::None:
            qDebug() << "state is still 'None'";
            return false;

        case ParameterGroup::Invalid:
            qDebug() << "state is 'Invalid'";
            return false;
        }
    }

    return true;
}

ParameterGroup SaxHandler::nameToState(QString name)
{
    QHash<QString, ParameterGroup> hash;

    hash["Infiltrationsfaktoren"] = ParameterGroup::Infiltrationsfaktoren;
    hash["Bagrovwerte"] = ParameterGroup::Bagrovwerte;
    hash["ErgebnisNachkommaStellen"] = ParameterGroup::Nachkomma;
    hash["Diverse"] = ParameterGroup::Diverse;
    hash["Gewaesserverdunstung" ] = ParameterGroup::GewVerd;
    hash["PotentielleVerdunstung"] = ParameterGroup::PotVerd;

    return name.isEmpty() ? ParameterGroup::None : hash[name];
}

void SaxHandler::setInfiltrationFactor(QString key, float value)
{
    initValues.setInfiltrationFactor(surfaceNameToIndex(key), value);
}

void SaxHandler::setBagrovValue(QString key, float value)
{
    initValues.setBagrovValue(surfaceNameToIndex(key), value);
}

int SaxHandler::surfaceNameToIndex(QString name)
{
    QHash <QString, int> hash;

    hash["Dachflaechen"] = 0;
    hash["Belaglsklasse1"] = 1;
    hash["Belaglsklasse2"] = 2;
    hash["Belaglsklasse3"] = 3;
    hash["Belaglsklasse4"] = 4;

    return hash[name];
}

void SaxHandler::setDigits(QString key, int value)
{
    if (key == "R")
        initValues.setDigitsTotalRunoff_R(value);
    else if (key == "ROW")
        initValues.setDigitsSurfaceRunoff_ROW(value);
    else if (key == "RI")
        initValues.setDigitsInfiltration_RI(value);
    else if (key == "RVOL")
        initValues.setDigitsTotalRunoffFlow_RVOL(value);
    else if (key == "ROWVOL")
        initValues.setDigitsSurfaceRunoffFlow_ROWVOL(value);
    else if (key == "RIVOL")
        initValues.setDigitsInfiltrationFlow_RIVOL(value);
    else if (key == "FLAECHE")
        initValues.setDigitsTotalArea_FLAECHE(value);
    else if (key == "VERDUNSTUNG")
        initValues.setDigitsEvaporation_VERDUNSTUN(value);
}

void SaxHandler::setDiverse(QString key, QString value)
{
    if (key == "BERtoZero")
        initValues.setIrrigationToZero(value == "true");
    else if (key == "NIEDKORRF")
        initValues.setPrecipitationCorrectionFactor(value.toFloat());
}

void SaxHandler::waterEvaporationEntry(const QXmlAttributes &attribs)
{
    QString bezirke = attribs.value("bezirke");
    QString eg = attribs.value("eg");

    initValues.putToHashOfType(bezirke, eg.toInt(), 13);
}

void SaxHandler::potentialEvaporationEntry(const QXmlAttributes &attribs)
{
    QString etp = attribs.value("etp");
    QString etps = attribs.value("etps");

    if (etp.length() > 0 || etps.length() > 0) {

        QString bezirke = attribs.value("bezirke");

        if (etp.length() > 0) {
            initValues.putToHashOfType(bezirke, etp.toInt(), 11);
        }

        if (etps.length() > 0) {
            initValues.putToHashOfType(bezirke, etps.toInt(), 12);
        }
    }
}

bool SaxHandler::fatalError(const QXmlParseException & /*exception*/)
{
    return false;
}
