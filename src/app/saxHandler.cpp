/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#include <QDebug>

#include "saxHandler.h"

SaxHandler::SaxHandler(InitValues &initValues):
    m_parameterGroup(ParameterGroup::None),
    initValues(initValues)
{}

bool SaxHandler::startElement(
    const QString &,
    const QString &,
    const QString &qName,
    const QXmlAttributes &attribs
)
{
    int digits;

    if (qName == "section") {
        m_parameterGroup = nameToParameterGroup(attribs.value("name"));
        return (m_parameterGroup != ParameterGroup::None && m_parameterGroup != ParameterGroup::Invalid);
    }

    if (qName == "item") {

        QString key = attribs.value("key");
        QString value = attribs.value("value");

        switch (m_parameterGroup) {

        case ParameterGroup::Infiltrationsfaktoren:
            initValues.setInfiltrationFactor(
                surfaceNameToIndex(key),
                value.toFloat()
            );
            break;

        case ParameterGroup::Bagrovwerte :
            initValues.setBagrovValue(
                surfaceNameToIndex(key),
                value.toFloat()
            );
            break;

        case ParameterGroup::Nachkomma :
            digits = value.toInt();
            if (key == "R")
                initValues.setDigitsTotalRunoff_R(digits);
            else if (key == "ROW")
                initValues.setDigitsSurfaceRunoff_ROW(digits);
            else if (key == "RI")
                initValues.setDigitsInfiltration_RI(digits);
            else if (key == "RVOL")
                initValues.setDigitsTotalRunoffFlow_RVOL(digits);
            else if (key == "ROWVOL")
                initValues.setDigitsSurfaceRunoffFlow_ROWVOL(digits);
            else if (key == "RIVOL")
                initValues.setDigitsInfiltrationFlow_RIVOL(digits);
            else if (key == "FLAECHE")
                initValues.setDigitsTotalArea_FLAECHE(digits);
            else if (key == "VERDUNSTUNG")
                initValues.setDigitsEvaporation_VERDUNSTUN(digits);
            break;

        case ParameterGroup::Diverse :
            if (key == "BERtoZero")
                initValues.setIrrigationToZero(value == "true");
            else if (key == "NIEDKORRF")
                initValues.setPrecipitationCorrectionFactor(value.toFloat());
            break;

        case ParameterGroup::GewVerd :
            potentialEvaporationEntry(attribs, "eg", 13);
            break;

        case ParameterGroup::PotVerd :
            potentialEvaporationEntry(attribs, "etp", 11);
            potentialEvaporationEntry(attribs, "etps", 12);
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

ParameterGroup SaxHandler::nameToParameterGroup(QString name)
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

void SaxHandler::potentialEvaporationEntry(
    const QXmlAttributes &attribs, QString name, int index
)
{
    QString element = attribs.value(name);

    if (element.length() > 0) {
        QString bezirke = attribs.value("bezirke");
        initValues.putToHashOfType(bezirke, element.toInt(), index);
    }
}

bool SaxHandler::fatalError(const QXmlParseException & /*exception*/)
{
    return false;
}
