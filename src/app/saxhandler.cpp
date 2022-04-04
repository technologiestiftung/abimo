/***************************************************************************
 *   Copyright (C) 2009 by Meiko Rachimow, Claus Rachimow                  *
 *   This file is part of Abimo 3.2                                        *
 *   Abimo 3.2 is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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

        QString name = attribs.value("name");

        if (name == "Infiltrationsfaktoren") {
            state = ParameterGroup::Infiltrationsfaktoren;
        }
        else if (name == "Bagrovwerte") {
            state = ParameterGroup::Bagrovwerte;
        }
        else if (name == "ErgebnisNachkommaStellen") {
            state = ParameterGroup::Nachkomma;
        }
        else if (name == "Diverse") {
            state = ParameterGroup::Diverse;
        }
        else if (name == "Gewaesserverdunstung") {
            state = ParameterGroup::GewVerd;
        }
        else if (name == "PotentielleVerdunstung") {
            state = ParameterGroup::PotVerd;
        }
    }

    if (qName == "item") {

        QString key = attribs.value("key");
        QString value = attribs.value("value");

        switch (state) {

        case ParameterGroup::Infiltrationsfaktoren:
            if (key == "Dachflaechen")
                initValues.setInfdach(value.toFloat());
            else if (key == "Belaglsklasse1")
                initValues.setInfbel1(value.toFloat());
            else if (key == "Belaglsklasse2")
                initValues.setInfbel2(value.toFloat());
            else if (key == "Belaglsklasse3")
                initValues.setInfbel3(value.toFloat());
            else if (key == "Belaglsklasse4")
                initValues.setInfbel4(value.toFloat());
            break;

        case ParameterGroup::Bagrovwerte :
            if (key == "Dachflaechen")
                initValues.setBagdach(value.toFloat());
            else if (key == "Belaglsklasse1")
                initValues.setBagbel1(value.toFloat());
            else if (key == "Belaglsklasse2")
                initValues.setBagbel2(value.toFloat());
            else if (key == "Belaglsklasse3")
                initValues.setBagbel3(value.toFloat());
            else if (key == "Belaglsklasse4")
                initValues.setBagbel4(value.toFloat());
            break;

        case ParameterGroup::Nachkomma :
            if (key == "R")
                initValues.setDecR(value.toInt());
            else if (key == "ROW")
                initValues.setDecROW(value.toInt());
            else if (key == "RI")
                initValues.setDecRI(value.toInt());
            else if (key == "RVOL")
                initValues.setDecRVOL(value.toInt());
            else if (key == "ROWVOL")
                initValues.setDecROWVOL(value.toInt());
            else if (key == "RIVOL")
                initValues.setDecRIVOL(value.toInt());
            else if (key == "FLAECHE")
                initValues.setDecFLAECHE(value.toInt());
            else if (key == "VERDUNSTUNG")
                initValues.setDecVERDUNSTUNG(value.toInt());
            break;

        case ParameterGroup::Diverse :
            if (key == "BERtoZero")
                initValues.setBERtoZero(value == "true");
            else if (key == "NIEDKORRF")
                initValues.setNiedKorrF(value.toFloat());
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
