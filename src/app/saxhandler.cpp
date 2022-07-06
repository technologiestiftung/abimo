/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#include <QDebug>

#include "saxhandler.h"

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
            setInfiltrationsfaktor(key, value.toFloat());
            break;

        case ParameterGroup::Bagrovwerte :
            setBagrovwert(key, value.toFloat());
            break;

        case ParameterGroup::Nachkomma :
            setNachkomma(key, value.toInt());
            break;

        case ParameterGroup::Diverse :
            setDivers(key, value);
            break;

        case ParameterGroup::GewVerd :
            gewVerdEntry(attribs);
            break;

        case ParameterGroup::PotVerd :
            potVerdEntry(attribs);
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
    if (name == "Infiltrationsfaktoren") {
        return ParameterGroup::Infiltrationsfaktoren;
    }

    if (name == "Bagrovwerte") {
        return ParameterGroup::Bagrovwerte;
    }

    if (name == "ErgebnisNachkommaStellen") {
        return ParameterGroup::Nachkomma;
    }

    if (name == "Diverse") {
        return ParameterGroup::Diverse;
    }

    if (name == "Gewaesserverdunstung") {
        return ParameterGroup::GewVerd;
    }

    if (name == "PotentielleVerdunstung") {
        return ParameterGroup::PotVerd;
    }

    if (name.isEmpty()) {
        return ParameterGroup::None;
    }

    return ParameterGroup::Invalid;
}

void SaxHandler::setInfiltrationsfaktor(QString key, float value)
{
    if (key == "Dachflaechen")
        initValues.setInfdach(value);
    else if (key == "Belaglsklasse1")
        initValues.setInfbel1(value);
    else if (key == "Belaglsklasse2")
        initValues.setInfbel2(value);
    else if (key == "Belaglsklasse3")
        initValues.setInfbel3(value);
    else if (key == "Belaglsklasse4")
        initValues.setInfbel4(value);
}

void SaxHandler::setBagrovwert(QString key, float value)
{
    if (key == "Dachflaechen")
        initValues.setBagdach(value);
    else if (key == "Belaglsklasse1")
        initValues.setBagbel1(value);
    else if (key == "Belaglsklasse2")
        initValues.setBagbel2(value);
    else if (key == "Belaglsklasse3")
        initValues.setBagbel3(value);
    else if (key == "Belaglsklasse4")
        initValues.setBagbel4(value);
}

void SaxHandler::setNachkomma(QString key, int value)
{
    if (key == "R")
        initValues.setDecR(value);
    else if (key == "ROW")
        initValues.setDecROW(value);
    else if (key == "RI")
        initValues.setDecRI(value);
    else if (key == "RVOL")
        initValues.setDecRVOL(value);
    else if (key == "ROWVOL")
        initValues.setDecROWVOL(value);
    else if (key == "RIVOL")
        initValues.setDecRIVOL(value);
    else if (key == "FLAECHE")
        initValues.setDecFLAECHE(value);
    else if (key == "VERDUNSTUNG")
        initValues.setDecVERDUNSTUNG(value);
}

void SaxHandler::setDivers(QString key, QString value)
{
    if (key == "BERtoZero")
        initValues.setBERtoZero(value == "true");
    else if (key == "NIEDKORRF")
        initValues.setNiedKorrF(value.toFloat());
}

void SaxHandler::gewVerdEntry(const QXmlAttributes &attribs)
{
    QString bezirke = attribs.value("bezirke");
    QString eg = attribs.value("eg");

    initValues.putToHash(bezirke, eg.toInt(), 13);
}

void SaxHandler::potVerdEntry(const QXmlAttributes &attribs)
{
    QString etp = attribs.value("etp");
    QString etps = attribs.value("etps");

    if (etp.length() > 0 || etps.length() > 0) {

        QString bezirke = attribs.value("bezirke");

        if (etp.length() > 0) {
            initValues.putToHash(bezirke, etp.toInt(), 11);
        }

        if (etps.length() > 0) {
            initValues.putToHash(bezirke, etps.toInt(), 12);
        }
    }
}

bool SaxHandler::fatalError(const QXmlParseException & /*exception*/)
{
    return false;
}
