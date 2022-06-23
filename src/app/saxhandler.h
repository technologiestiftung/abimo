/***************************************************************************
 * For copyright information please see COPYRIGHT.txt in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef SAXHANDLER_H
#define SAXHANDLER_H

#include <QXmlDefaultHandler>
#include <QXmlAttributes>
#include <QXmlParseException>

#include "initvalues.h"

enum struct ParameterGroup {
    None = 0,
    Infiltrationsfaktoren = 1,
    Bagrovwerte = 2,
    Nachkomma = 3,
    Diverse = 4,
    GewVerd = 5,
    PotVerd = 6,
    Invalid = 99
};

class SaxHandler : public QXmlDefaultHandler
{

public:
    SaxHandler(InitValues &initValues);
    bool startElement(
        const QString &namespaceURI,
        const QString &localName,
        const QString &qName,
        const QXmlAttributes &attribs
    );
    bool fatalError(const QXmlParseException &exception);

private:
    ParameterGroup state;
    InitValues &initValues;
    void gewVerdEntry(const QXmlAttributes &attribs);
    void potVerdEntry(const QXmlAttributes &attribs);
    static ParameterGroup nameToState(QString name);
    void setInfiltrationsfaktor(QString key, float value);
    void setBagrovwert(QString key, float value);
    void setNachkomma(QString key, int value);
    void setDivers(QString key, QString value);

};

#endif
