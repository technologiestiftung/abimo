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

#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QString>
#include <QTextStream>

#include "dbaseReader.h"
#include "initvalues.h"
#include "pdr.h"

class Calculation: public QObject
{
    Q_OBJECT

public:
    Calculation(DbaseReader & dbR, InitValues & init, QTextStream & protoStream);
    bool calc(QString fileOut, bool debug = false);
    long getProtCount();
    long getKeineFlaechenAngegeben();
    long getNutzungIstNull();
    int getTotalRecWrite();
    int getTotalRecRead();
    int getTotalBERtoZeroForced();
    QString getError();
    void stop();
    static void calculate(QString inputFile, QString configFile, QString outputFile, bool debug = false);

signals:
    void processSignal(int, QString);

private:
    const static float iTAS[];
    const static float inFK_S[];
    const static float ijkr_S[];
    InitValues & initValues;
    QTextStream & protokollStream;
    DbaseReader & dbReader;
    PDR ptrDA;
    QString error;

    // Anzahl der Protokolleintraege
    long protcount;

    // Anzahl der nicht berechneten Flaechen
    long keineFlaechenAngegeben;

    long nutzungIstNull;

    // ******vorlaeufig aus Teilblock 0 wird fuer die Folgeblocks genommen
    float regenja, regenso;

    // Abfluesse nach Bagrov fuer N1 bis N4
    float RDV, R1V, R2V, R3V, R4V;

    float RUV;

    // Regenwasserabfluss in Qubikzentimeter pro Sekunde
    float ROWVOL;

    // unterirdischer Gesamtabfluss in qcm/s
    float RIVOL;

    // Gesamtabfluss in qcm/s
    float RVOL;

    // potentielle Aufstiegshoehe
    float TAS;

    // Niederschlags-Korrekturfaktor
    float niedKorrFaktor;

    // Feldlaenge von iTAS
    int lenTAS;

    // Feldlaenge von inFK_S
    int lenS;

    // total written records
    int totalRecWrite;

    // total read records
    int totalRecRead;

    // Anzahl der Records fuer die BER == 0 gesetzt werden musste
    int totalBERtoZeroForced;

    // to stop calc
    bool weiter;

    // functions
    float getNUV(PDR &B);
    float getF (float wa);
    float getG02 (int nFK);
    void getNUTZ(int nutz, int typ, int f30, int f150, QString codestr);
    void setUsageYieldIrrigation(int usage, int type, QString code);
    void logNotDefined(QString code, int type);
    void getKLIMA(int bez, QString codestr);
};

#endif
