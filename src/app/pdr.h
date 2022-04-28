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

#ifndef PDR_H
#define PDR_H

#include <QString>

// Descriptions from here:
// https://www.berlin.de/umweltatlas/_assets/wasser/wasserhaushalt/
//   de-abbildungen/maxsize_405ab3ac7c0e2104d3a03c317ddd93f0_a213_02a.jpg
enum struct Usage: char {
    // landwirtschaftliche Nutzflaeche (einschliesslich Graland)
    agricultural_L = 'L',
    // forstliche Nutzflaeche (Annahme gleichmaessig verteilter Bestandsaltersgruppen)
    forested_W = 'W',
    // Gewaesserflaeche
    waterbody_G = 'G',
    // gaertnerische Nutzflaeche (programmintern: BER = 75 mm/a)
    horticultural_K = 'K',
    // vegetationslose Flaeche
    vegetationless_D = 'D',
    // initial value
    unknown = '?'
};

struct UsageResult {
    int tupleIndex;
    QString message;
};

struct UsageTuple {
    Usage usage;
    int yield;
    int irrigation;
};

class PDR
{
public:
    PDR();
    void setUsageYieldIrrigation(Usage usage, int yield = 0, int irrigation = 0);
    void setUsageYieldIrrigation(UsageTuple tuple);
    static float estimateWaterHoldingCapacity(int f30, int f150, bool isForest);
    static int estimateDaysOfGrowth(Usage usage, int yield);

    // Elementnummer EB_INDEX neu
    unsigned wIndex;

    // nFK-Wert (ergibt sich aus Bodenart) ID_NFK neu
    // water holding capacity (= nutzbare Feldkapazitaet)
    float nFK;

    // Flurabstandswert [m] ID_FLW 4.1 N
    float FLW;

    // Hauptnutzungsform [L,W,G,B,K,D] ID_NUT 001 C
    Usage NUT;

    // Langjaehriger MW des Gesamtabflusses [mm/a] 004 N
    int R;

    // Langjaehriger MW des Regenwasserabflusses [mm/a] 003 N
    int ROW;

    // Langjaehriger MW des unterird. Abflusses [mm/a] 004 N
    int RI;

    // Versiegelungsgrad bebauter Flaechen [%] ID_VER 002 N
    int VER;

    // Ertragsklasse landw. Nutzflaechen ID_ERT 002 N
    int ERT;

    // j. Beregnungshoehe landw. Nutzfl. [mm/a] ID_BER 003 N
    int BER;

    // Niederschlag <MD-Wert> [mm/a] ID_PMD 003 N
    float P1;

    // l-j. MW der pot. Verdunstung [mm/a] ID_ETP 003 N
    int ETP;

    // Kapillarer Aufstieg pro Jahr ID_KR neu
    int KR;

    // Sommer-Niederschlag ID_PS neu
    float P1S;

    // potentielle Verdunstung im Sommer ID_ETPS neu
    int ETPS;
};

#endif
