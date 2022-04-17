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

#include <math.h>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include "bagrov.h"
#include "calculation.h"
#include "config.h"
#include "constants.h"
#include "dbaseReader.h"
#include "dbaseWriter.h"
#include "effectivenessunsealedsurfaces.h"
#include "helpers.h"
#include "initvalues.h"
#include "pdr.h"

// potential ascent rate TAS (column labels for matrix 'Calculation::ijkr_S')
const float Calculation::iTAS[] = {
    0.1F, 0.2F, 0.3F, 0.4F, 0.5F, 0.6F, 0.7F, 0.8F,
    0.9F, 1.0F, 1.2F, 1.4F, 1.7F, 2.0F, 2.3F
};

// soil type unknown - default soil type used in the following: sand

// Usable field capacity nFK (row labels for matrix 'Calculation::ijkr_S')
const float Calculation::inFK_S[] = {
    8.0F, 9.0F, 14.0F, 14.5F, 15.5F, 17.0F, 20.5F
};

/* Mean potential capillary rise rate kr [mm/d] of a summer season depending on:
 * potential ascent rate TAS (one column each) and
 * usable field capacity nFK (one row each) */
const float Calculation::ijkr_S[] = {
    7.0F, 6.0F, 5.0F, 1.5F, 0.5F, 0.2F, 0.1F, 0.0F, 0.0F, 0.0F,  0.0F , 0.0F, 0.0F , 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 5.0F, 3.0F, 1.2F, 0.5F, 0.2F, 0.1F, 0.0F,  0.0F , 0.0F, 0.0F , 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 3.0F, 1.5F, 0.7F, 0.3F, 0.15F, 0.1F , 0.0F, 0.0F , 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 3.0F, 2.0F, 1.0F, 0.7F, 0.4F,  0.15F, 0.1F, 0.0F , 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 4.5F, 2.5F, 1.5F, 0.7F, 0.4F,  0.15F, 0.1F, 0.0F , 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 5.0F, 3.5F, 2.0F, 1.5F, 0.8F,  0.3F , 0.1F, 0.05F, 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 6.0F, 5.0F, 5.0F, 5.0F, 3.0F, 2.0F,  1.0F , 0.5F, 0.15F, 0.0F, 0.0F
};

Calculation::Calculation(DbaseReader& dbR, InitValues & init, QTextStream & protoStream):
    initValues(init),
    protokollStream(protoStream),
    dbReader(dbR),
    protcount(0),
    regenja(0),
    regenso(0),
    RDV(0),
    R1V(0),
    R2V(0),
    R3V(0),
    R4V(0),
    RUV(0),
    ROWVOL(0),
    RIVOL(0),
    RVOL(0),
    TAS(0),
    lenTAS(15),
    lenS(7),
    totalRecWrite(0),
    totalRecRead(0),
    totalBERtoZeroForced(0),
    weiter(true)
{
}

void Calculation::stop()
{
    weiter = false;
}

long Calculation::getProtCount()
{
    return protcount;
}

long Calculation::getKeineFlaechenAngegeben()
{
    return keineFlaechenAngegeben;
}

int Calculation::getTotalRecWrite()
{
    return totalRecWrite;
}

int Calculation::getTotalRecRead()
{
    return totalRecRead;
}

int Calculation::getTotalBERtoZeroForced()
{
    return totalBERtoZeroForced;
}

long Calculation::getNutzungIstNull()
{
    return nutzungIstNull;
}

QString Calculation::getError()
{
    return error;
}

/**
 =======================================================================================================================
    Diese Funktion importiert die Datensaetze aus der DBASE-Datei FileName in das DA Feld ein
    (GWD-Daten). Parameter: out-file Rueckgabewert: BOOL TRUE, wenn das Einlesen der Datei
    erfolgreich war.
 =======================================================================================================================
 */
bool Calculation::calc(QString fileOut, bool debug)
{
    // Current Abimo record (represents one row of the input dbf file)
    abimoRecord record;

    // variables for calculation
    int index = 0;
    
    // Versiegelungsgrad Dachflaechen / sonst. versiegelte Flaechen / Strassen
    // vegree of sealing of roof surfaces / other sealed surfaces / roads
    float vgd, vgb, vgs;

    // Kanalisierungsgrad Dachflaechen / sonst. versiegelte Flaechen / Strassen
    // degree of canalization for roof surfaces / other sealed surfaces / roads
    float kd, kb, ks;
    
    // Anteil der jeweiligen Belagsklasse
    // share of respective pavement class
    float bl1, bl2, bl3, bl4;

    // Anteil der jeweiligen Strassenbelagsklasse
    // share of respective road pavement class
    float bls1, bls2, bls3, bls4;

    // Gesamtflaeche Bebauung / Strasse
    // total area of building development / road
    float fb, fs;

    // Verhaeltnis Bebauungsflaeche / Strassenflaeche zu Gesamtflaeche (ant = Anteil)
    // share of building development area / road area to total area
    float fbant, fsant;
    
    // Abflussvariablen der versiegelten Flaechen
    // runoff variables of sealed surfaces
    float row1, row2, row3, row4;
    
    // Infiltrationsvariablen der versiegelten Flaechen
    // infiltration variables of sealed surfaces
    float ri1, ri2, ri3, ri4;
    
    // Abfluss- / Infiltrationsvariablen der Dachflaechen
    // runoff- / infiltration variables of roof surfaces
    float rowd, rid;
    
    // Abfluss- / Infiltrationsvariablen unversiegelter Strassenflaechen
    // runoff- / infiltration variables of unsealed road surfaces
    float rowuvs, riuvs;
    
    // Infiltration unversiegelter Flaechen
    // infiltratio of unsealed areas
    float riuv;
    
    // float-Zwischenwerte
    // float interm values
    float r, ri, row;
    int k;

    // count protocol entries
    protcount = 0L;
    keineFlaechenAngegeben = 0L;
    nutzungIstNull = 0L;

    // first entry into protocol
    DbaseWriter writer(fileOut, initValues);

    // get the number of rows in the input data ?
    totalRecRead = dbReader.getNumberOfRecords();

    // loop over all block partial areas (records) of input data
    for (k = 0; k < totalRecRead; k++) {

        if (! weiter) {
            protokollStream << "Berechnungen abgebrochen.\r\n";
            return true;
        }

        ptrDA.wIndex = index;

        // Fill record with data from row k
        dbReader.fillRecord(k, record, debug);

        // NUTZUNG = integer representing the type of area usage for each block partial area
        if (record.NUTZUNG != 0) {

            // CODE: unique identifier for each block partial area

            // precipitation for entire year 'regenja' and for only summer season 'regenso'
            regenja = record.REGENJA; /* Jetzt regenja,-so OK */
            regenso = record.REGENSO;

            // depth to groundwater table 'FLUR'
            ptrDA.FLW = record.FLUR;

            getNUTZ(
                record.NUTZUNG,
                record.TYP,      // structure type
                record.FELD_30,  // field capacity [%] for 0- 30cm below ground level
                record.FELD_150, // field capacity [%] for 0-150cm below ground level
                record.CODE
            );

            /* cls_6a: an dieser Stelle muss garantiert werden, dass f30 und f150
               als Parameter von getNUTZ einen definierten Wert erhalten und zwar 0.

               FIXED: alle Werte sind definiert... wenn keine 0, sondern nichts bzw. Leerzeichen
               angegeben wurden, wird nun eine 0 eingesetzt
               aber eigentlich war das auch schon so ... ???
            */

            // Bagrov-calculation for sealed surfaces
            getKLIMA(record.BEZIRK, record.CODE);

            // share of roof area [%] 'PROBAU'
            vgd = record.PROBAU_fraction;
          
            // share of other sealed areas (e.g. Hofflaechen) and calculate total sealed area
            vgb = record.PROVGU_fraction;
            ptrDA.VER = INT_ROUND(vgd * 100 + vgb * 100);
            
            // share of sealed road area
            vgs = record.VGSTRASSE_fraction;
          
            // degree of canalization for roof / other sealed areas / sealed roads
            kd = record.KAN_BEB_fraction;
            kb = record.KAN_VGU_fraction;
            ks = record.KAN_STR_fraction;
          
            // share of each pavement class for surfaces except roads of block area
            bl1 = record.BELAG1_fraction;
            bl2 = record.BELAG2_fraction;
            bl3 = record.BELAG3_fraction;
            bl4 = record.BELAG4_fraction;
          
            // share of each pavement class for roads of block area
            bls1 = record.STR_BELAG1_fraction;
            bls2 = record.STR_BELAG2_fraction;
            bls3 = record.STR_BELAG3_fraction;
            bls4 = record.STR_BELAG4_fraction;
          
            fb = record.FLGES;
            fs = record.STR_FLGES;
            
            // if sum of total building development area and roads area is inconsiderably small
            // it is assumed, that the area is unknown and 100 % building development area will be given by default
            if (fb + fs < 0.0001)
            {
                //*protokollStream << "\r\nDie Flaeche des Elements " + record.CODE + " ist 0 \r\nund wird automatisch auf 100 gesetzt\r\n";
                protcount++;
                keineFlaechenAngegeben++;
                fb = 100.0F;
            }

            // fbant = Verhaeltnis Bebauungsflaeche zu Gesamtflaeche
            // fbant = ratio of building development area to total area
            fbant = fb / (fb + fs);
            
            // fsant = Verhaeltnis Strassenflaeche zu Gesamtflaeche
            // fsant = ratio of roads area to total area
            fsant = fs / (fb + fs);

            // Runoff for sealed surfaces
            /* cls_1: Fehler a:
               rowd = (1.0F - initValues.getInfdach()) * vgd * kb * fbant * RDV;
               richtige Zeile folgt (kb ----> kd)
            */
            
            /*  Legende der Abflussberechnung der 4 Belagsklassen bzw. Dachklasse:
                rowd / rowx: Abfluss Dachflaeche / Abfluss Belagsflaeche x
                infdach / infbelx: Infiltrationsparameter Dachfl. / Belagsfl. x
                belx: Anteil Belagsklasse x
                blsx: Anteil Strassenbelagsklasse x
                vgd / vgb: Anteil versiegelte Dachfl. / sonstige versiegelte Flaeche zu Gesamtblockteilflaeche
                kd / kb / ks: Grad der Kanalisierung Dach / sonst. vers. Fl. / Strassenflaechen
                fbant / fsant: ?
                RDV / RxV: Gesamtabfluss versiegelte Flaeche
            */
            rowd = (1.0F - initValues.getInfdach()) * vgd * kd * fbant * RDV;
            row1 = (1.0F - initValues.getInfbel1()) * (bl1 * kb * vgb * fbant + bls1 * ks * vgs * fsant) * R1V;
            row2 = (1.0F - initValues.getInfbel2()) * (bl2 * kb * vgb * fbant + bls2 * ks * vgs * fsant) * R2V;
            row3 = (1.0F - initValues.getInfbel3()) * (bl3 * kb * vgb * fbant + bls3 * ks * vgs * fsant) * R3V;
            row4 = (1.0F - initValues.getInfbel4()) * (bl4 * kb * vgb * fbant + bls4 * ks * vgs * fsant) * R4V;

            // Infiltration for sealed surfaces
            rid = (1 - kd) * vgd * fbant * RDV;
            ri1 = (bl1 * vgb * fbant + bls1 * vgs * fsant) * R1V - row1;
            ri2 = (bl2 * vgb * fbant + bls2 * vgs * fsant) * R2V - row2;
            ri3 = (bl3 * vgb * fbant + bls3 * vgs * fsant) * R3V - row3;
            ri4 = (bl4 * vgb * fbant + bls4 * vgs * fsant) * R4V - row4;
            
            // consider unsealed road surfaces as pavement class 4
            rowuvs = 0.0F;                   /* old: 0.11F * (1-vgs) * fsant * R4V; */
            riuvs = (1 - vgs) * fsant * R4V; /* old: 0.89F * (1-vgs) * fsant * R4V; */

            // runoff for unsealed surfaces rowuv = 0
            riuv = (100.0F - (float) ptrDA.VER) / 100.0F * RUV;

            // calculate runoff 'row' for entire block patial area (FLGES+STR_FLGES)
            row = (row1 + row2 + row3 + row4 + rowd + rowuvs); // mm/a
            ptrDA.ROW = INT_ROUND(row);
            
            // calculate volume 'rowvol' from runoff
            ROWVOL = row * 3.171F * (fb + fs) / 100000.0F;     // qcm/s
            
            // calculate infiltration rate 'ri' for entire block partial area
            ri = (ri1 + ri2 + ri3 + ri4 + rid + riuvs + riuv); // mm/a
            ptrDA.RI = INT_ROUND(ri);
            
            // calculate volume 'rivol' from infiltration rate
            RIVOL = ri * 3.171F * (fb + fs) / 100000.0F;       // qcm/s
            
            // calculate total system losses 'r' due to runoff and infiltration for entire block partial area
            r = row + ri;
            ptrDA.R = INT_ROUND(r);
            
            // calculate volume of system losses 'rvol'due to runoff and infiltration
            RVOL = ROWVOL + RIVOL;

            // calculate total area of building development area as well as roads area
            float flaeche1 = fb + fs;
// cls_5b:
            // calculate evaporation 'verdunst' by subtracting the sum of
            // runoff and infiltration 'r' from precipitation of entire year
            // 'regenja' multiplied by correction factor 'niedKorrFaktor'
            float verdunst = (regenja * initValues.getNiedKorrF()) - r;

            // write the calculated variables into respective fields
            writer.addRecord();
            writer.setRecordField("CODE", record.CODE);
            writer.setRecordField("R", r);
            writer.setRecordField("ROW", row);
            writer.setRecordField("RI", ri);
            writer.setRecordField("RVOL", RVOL);
            writer.setRecordField("ROWVOL", ROWVOL);
            writer.setRecordField("RIVOL", RIVOL);
            writer.setRecordField("FLAECHE", flaeche1);
// cls_5c:
            writer.setRecordField("VERDUNSTUN", verdunst);

            index++;
        }
        else {
            nutzungIstNull++;

        }

        /* cls_2: Hier koennten falls gewuenscht die Flaechen dokumentiert werden,
           deren NUTZUNG=NULL (siehe auch cls_3)
        */

        emit processSignal((int)((float) k / (float) totalRecRead * 50.0), "Berechne");
    }

    totalRecWrite = index;

    emit processSignal(50, "Schreibe Ergebnisse.");

    if (!writer.write()) {
        protokollStream << "Error: "+ writer.getError() +"\r\n";
        error = "Fehler beim Schreiben der Ergebnisse.\n" + writer.getError();
        return false;
    }

    return true;
}

/*
 =======================================================================================================================
    FIXME:
 =======================================================================================================================
 */

void Calculation::getNUTZ(int nutz, int typ, int f30, int f150, QString code)
{
    // mittlere pot. kapillare Aufstiegsrate d. Sommerhalbjahres
    float kr;

    /*
     * Feldlaengen von iTAS und inFK_S, L, T, U ;
     * extern int lenTAS, lenS, lenL, lenT, lenU;
     */

    // declaration of yield power (ERT) and irrigation (BER) for agricultural or gardening purposes
    setUsageYieldIrrigation(nutz, typ, code);

    if (ptrDA.NUT != 'G')
    {
        /* pot. Aufstiegshoehe TAS = FLUR - mittl. Durchwurzelungstiefe TWS */
        TAS = ptrDA.FLW - Config::getTWS(ptrDA.ERT, ptrDA.NUT);

        /* Feldkapazitaet */
        /* cls_6b: der Fall der mit NULL belegten FELD_30 und FELD_150 Werte
           wird hier im erten Fall behandelt - ich erwarte dann den Wert 0 */
        ptrDA.nFK = PDR::estimateWaterHoldingCapacity(f30, f150, ptrDA.NUT == 'W');

        /*
         * mittlere pot. kapillare Aufstiegsrate kr (mm/d) des Sommerhalbjahres ;
         * switch (bod) { case S: case U: case L: case T: case LO: case HN: } wird
         * eingefuegt, wenn die Bodenart in das Zahlenmaterial aufgenommen wird vorlaeufig
         * wird Sande angenommen ;
         * Sande
         */
        kr = (TAS <= 0.0) ?
            7.0F :
            ijkr_S[
                Helpers::index(TAS, iTAS, lenTAS) +
                Helpers::index(ptrDA.nFK, inFK_S, lenS) * lenTAS
            ];

        /* mittlere pot. kapillare Aufstiegsrate kr (mm/d) des Sommerhalbjahres */
        ptrDA.KR = (int) (PDR::estimateDaysOfGrowth(ptrDA.NUT, ptrDA.ERT) * kr);
    }

    if (initValues.getBERtoZero() && ptrDA.BER != 0) {
        //*protokollStream << "Erzwinge BER=0 fuer Code: " << code << ", Wert war:" << ptrDA.BER << " \r\n";
        totalBERtoZeroForced++;
        ptrDA.BER = 0;
    }
}

void Calculation::setUsageYieldIrrigation(int usage, int type, QString code)
{
    t_usageYieldIrrigation tuples[16];

    // Define all different value combinations
    // (usage - NUT, yield power - ERT, irrigation - BER)
    // NUT = 'K': gardening
    // NUT = 'L': agricultural land use

    tuples[ 0] = {'D',  1,   0};
    tuples[ 1] = {'G',  0,   0};
    tuples[ 2] = {'K', 40,  75};
    tuples[ 3] = {'L', 10,   0};
    tuples[ 4] = {'L', 25,   0};
    tuples[ 5] = {'L', 30,   0};
    tuples[ 6] = {'L', 35,   0};
    tuples[ 7] = {'L', 40,   0};
    tuples[ 8] = {'L', 45,   0};
    tuples[ 9] = {'L', 45,  50};
    tuples[10] = {'L', 50,   0};
    tuples[11] = {'L', 50, 100};
    tuples[12] = {'L', 50,  50};
    tuples[13] = {'L', 55,  75};
    tuples[14] = {'L', 60,   0};
    tuples[15] = {'W',  0,   0};

    // index of tuple to be applied
    int id = -1;

    switch (usage)
    {
        case 10:
        case 21:
        case 22:
        case 23:
        case 30: switch (type) {
            case  1: id = 6; break;
            case  2: id = 6; break;
            case  3: id = 8; break;
            case  4: id = 7; break;
            case  5: id = 7; break;
            case  6: id = 7; break;
            case  7: id = 6; break;
            case  8: id = 6; break;
            case  9: id = 7; break;
            case 10: id = 7; break;
            case 11: id = 6; break;
            case 21: id = 8; break;
            case 22: id = 2; break;
            case 23: id = 2; break;
            case 24: id = 13; break;
            case 25: id = 2; break;
            case 26: id = 7; break;
            case 29: id = 5; break;
            case 33: id = 6; break;
            case 38: id = 6; break;
            case 39: id = 6; break;
            case 71: id = 8; break;
            case 72: id = 10; break;
            case 73: id = 10; break;
            case 74: id = 10; break;
            default: id = 10; logNotDefined(code, 72);
        }
        break;

        case 40: switch (type) {
            case 30: id = 6; break;
            case 31: id = 5; break;
            default: id = 5; logNotDefined(code, 31);
        }
        break;

        case 50: switch (type) {
            case 12: id = 8; break;
            case 13: id = 10; break;
            case 14: id = 10; break;
            case 28: id = 7; break;
            case 41: id = 7; break;
            case 42: id = 6; break;
            case 43: id = 6; break;
            case 44: id = 9; break;
            case 45: id = 7; break;
            case 46: id = 12; break;
            case 47: id = 8; break;
            case 49: id = 9; break;
            case 50: id = 9; break;
            case 51: id = 8; break;
            case 60: id = 8; break;
            default: id = 8; logNotDefined(code, 60);
        }
        break;

        case 60: id = 8; break;

        case 70: switch (type) {
            case 59: id = 2; break;
            default: id = 2; logNotDefined(code, 59);
        }
        break;

        case 80: switch (type) {
            case 91: id = 7; break;
            case 92: id = 4; break;
            case 93: id = 5; break;
            case 94: id = 5; break;
            case 99: id = 3; break;
            default: id = 3; logNotDefined(code, 99);
        }
        break;

        case 90: switch (type) {
            case 98: id = 0; break;
            default: id = 0; logNotDefined(code, 98);
        }
        break;

        case 100: switch (type) {
            case 55: id = 15; break;
            default: id = 15; logNotDefined(code, 55);
        }
        break;

        case 101: id = 15; break;
        case 102: id = 14; break;
        case 110: id = 1; break;
        case 121: id = 7; break;
        case 122: id = 6; break;
        case 130: id = 12; break;
        case 140: id = 10; break;
        case 150: id = 11; break;
        case 160: id = 2; break;
        case 161: id = 2; break;
        case 162: id = 2; break;
        case 170: id = 3; break;
        case 171: id = 0; break;
        case 172: id = 7; break;
        case 173: id = 8; break;
        case 174: id = 14; break;
        case 180: id = 10; break;
        case 190: id = 7; break;
        case 200: id = 12; break;
        default:
            protokollStream << "\r\nDiese  Meldung sollte nie erscheinen: \r\nNutzung nicht definiert fuer Element " + code + "\r\n";
    }

    if (id >= 0) {
        ptrDA.setUsageYieldIrrigation(tuples[id]);
    }
}

void Calculation::logNotDefined(QString code, int type)
{
    protokollStream << "\r\nNutzungstyp nicht definiert fuer Element " +
                       code +
                       "\r\nTyp=" +
                       QString::number(type) +
                       " angenommen\r\n";
    protcount++;
}

/*
 =======================================================================================================================
    FIXME:
 =======================================================================================================================
 */
void Calculation::getKLIMA(int bez, QString code)
{
    /* Effektivitaetsparameter */
    float nd = initValues.getBagdach();
    float n1 = initValues.getBagbel1();
    float n2 = initValues.getBagbel2();
    float n3 = initValues.getBagbel3();
    float n4 = initValues.getBagbel4();
    float n, bag, zw;

    /* ep = potential evaporation,
       p = prepcipitation at ground level,
       x = ratio of precipitation to potential evaporation,
       y = ratio of real evaporation to potential evaporation,
       etr = real evapotranspiration */
    float ep, p, x, y, etr;

    QString bezString;
    bezString.setNum(bez);

    /*
     * spaeter zeizusaetzliche Parameter Hier ;
     * ptrDA.P1 = p1;
     * * ptrDA.PS = ps;
     */
    ptrDA.P1 = regenja;
    ptrDA.P1S = regenso;

    // parameter for the city districts
    if (ptrDA.NUT == 'G')
    {
        if(initValues.hashEG.contains(bez)) {
            //take from xml
            ptrDA.ETP = initValues.hashEG.value(bez);
        } else {
            //default
            ptrDA.ETP = initValues.hashEG.contains(0) ? initValues.hashEG.value(0) : 775;
            QString egString;
            egString.setNum(ptrDA.ETP);
            protokollStream << "\r\nEG unbekannt fuer " + code + " von Bezirk " + bezString + "\r\nEG=" + egString + " angenommen\r\n";
            protcount++;
        }
    }
    else
    {
        if (initValues.hashETP.contains(bez)) {
            //take from xml
            ptrDA.ETP = initValues.hashETP.value(bez);
        } else {
            //default
            ptrDA.ETP = initValues.hashETP.contains(0) ? initValues.hashETP.value(0) : 660;
            QString etpString;
            etpString.setNum(ptrDA.ETP);
            protokollStream << "\r\nETP unbekannt fuer " + code + " von Bezirk " + bezString + "\r\nETP=" + etpString + " angenommen\r\n";
            protcount++;
        }

        if (initValues.hashETPS.contains(bez)) {
            //take from xml
            ptrDA.ETPS = initValues.hashETPS.value(bez);
        } else {
            //default
            ptrDA.ETPS = initValues.hashETPS.contains(0) ? initValues.hashETPS.value(0) : 530;
            QString etpsString;
            etpsString.setNum(ptrDA.ETPS);
            protokollStream << "\r\nETPS unbekannt fuer " + code + " von Bezirk " + bezString + "\r\nETPS=" + etpsString + " angenommen\r\n";
            protcount++;
        }
    }

    // declaration potential evaporation ep and precipitation p
    ep = (float) ptrDA.ETP;                /* Korrektur mit 1.1 gestrichen */
    p = (float) ptrDA.P1 * initValues.getNiedKorrF(); /* ptrDA.KF */

    /*
     * Berechnung der Abfluesse RDV und R1V bis R4V fuer versiegelte
     * Teilflaechen und unterschiedliche Bagrovwerte ND und N1 bis N4
     */

    // ratio precipitation to potential evaporation
    x = p / ep;

    Bagrov bagrovObj;

    /* Berechnung des Abflusses RxV fuer versiegelte Teilflaechen mittels
       Umrechnung potentieller Verdunstungen ep zu realen über Umrechnungsfaktor y und
       subtrahiert von Niederschlag p */
    bagrovObj.nbagro(&nd, &y, &x);
    RDV = p - y * ep;
    bagrovObj.nbagro(&n1, &y, &x);
    R1V = p - y * ep;
    bagrovObj.nbagro(&n2, &y, &x);
    R2V = p - y * ep;
    bagrovObj.nbagro(&n3, &y, &x);
    R3V = p - y * ep;
    bagrovObj.nbagro(&n4, &y, &x);
    R4V = p - y * ep;

    // Calculate runoff RUV for unsealed partial surfaces
    if (ptrDA.NUT == 'G')
        RUV = p - ep;
    else
    {
        // Determine effectiveness parameter bag for unsealed surfaces
        n = EffectivenessUnsealedSurfaces::getNUV(ptrDA); /* Modul Raster abgespeckt */

        if (ptrDA.P1S > 0 && ptrDA.ETPS > 0)
        {
            zw = (float) (ptrDA.P1S + ptrDA.BER + ptrDA.KR) / ptrDA.ETPS;
            bag = getSummerModificationFactor(zw) * n;
        }
        else
            bag = n;
        x = (p + ptrDA.KR + ptrDA.BER) / ep;
        bagrovObj.nbagro(&bag, &y, &x);

        if (TAS < 0)
            etr = (ep - y * ep) * (float) exp(ptrDA.FLW / TAS) + y * ep;
        else
            etr = y * ep;

        RUV = p - etr;
    }
}


// ============================================================================
// Get factor to be applied for "summer"
// ============================================================================
float Calculation::getSummerModificationFactor(float wa)
{
    const float watab[] =
    {
        0.45F, 0.50F, 0.55F, 0.60F, 0.65F, 0.70F, 0.75F, // 0 ..  6
        0.80F, 0.85F, 0.90F, 0.95F, 1.00F, 1.05F, 1.10F  // 7 .. 13
    };

    const float Ftab[] =
    {
        0.65F, 0.75F, 0.82F, 0.90F, 1.00F, 1.06F, 1.15F, // 0 ..  6
        1.22F, 1.30F, 1.38F, 1.47F, 1.55F, 1.63F, 1.70F  // 7 .. 13
    };

    return Helpers::interpolate(wa, watab, Ftab, 14);
}

void Calculation::calculate(QString inputFile, QString configFile, QString outputFile, bool debug)
{
    // Open the input file and read the raw (text) values into the dbReader object
    DbaseReader dbReader(inputFile);

    if (!dbReader.checkAndRead()) {
        abort();
    }

    // Update default initial values with values given in configFile
    InitValues initValues;

    if (configFile.isEmpty()) {
        qDebug() << "No config file given -> Using default values";
    }
    else {
        qDebug() << "Using configuration file:" << configFile;

        QString errorMessage = InitValues::updateFromConfig(initValues, configFile);
        if (!errorMessage.isEmpty()) {
            qDebug() << "Error in updateFromConfig: " << errorMessage;
            abort();
        }
    }

    QFile logHandle(Helpers::defaultLogFileName(outputFile));
    Helpers::openFileOrAbort(logHandle, QFile::WriteOnly);

    QTextStream logStream(&logHandle);

    Calculation calculator(dbReader, initValues, logStream);

    bool success = calculator.calc(outputFile, debug);

    if (!success) {
        qDebug() << "Error in calc(): " << calculator.getError();
        abort();
    }

    logHandle.close();
}

/*
usage_groups:
usage_group,usages
g1,10|21|22|23|30

{usage_group,type,id}
{g1,1,6}
{g1,2,6}
{g1,3,8}
{g1,4,7}
{g1,5,7}
{g1,6,7}
{g1,7,6}
{g1,8,6}
{g1,9,7}
{g1,10,7}
{g1,11,6}
{g1,21,8}
{g1,22,2}
{g1,23,2}
{g1,24,13}
{g1,25,2}
{g1,26,7}
{g1,29,5}
{g1,33,6}
{g1,38,6}
{g1,39,6}
{g1,71,8}
{g1,72,10}
{g1,73,10}
{g1,74,10}
{g1,-72,10} -> logNotDefined(code, 72);

{40,30,6}
{40,31,5}
{40,-31,5} -> logNotDefined(code, 31);

{50,12,8}
{50,13,10}
{50,14,10}
{50,28,7}
{50,41,7}
{50,42,6}
{50,43,6}
{50,44,9}
{50,45,7}
{50,46,12}
{50,47,8}
{50,49,9}
{50,50,9}
{50,51,8}
{50,60,8}
{50,-60,8} -> logNotDefined(code, 60);

{60,*,8}

{70,59,2}
{70,-59,2} -> logNotDefined(code, 59);

{80,91,7}
{80,92,4}
{80,93,5}
{80,94,5}
{80,99,3}
{80,-99,3} -> logNotDefined(code, 99);

{90,98,0}
{90,-98,0) -> logNotDefined(code, 98);

{100,55,15}
{100,-55,15} -> logNotDefined(code, 55);

{101,*,15}
{102,*,14}
{110,*,1}
{121,*,7}
{122,*,6}
{130,*,12}
{140,*,10}
{150,*,11}
{160,*,2}
{161,*,2}
{162,*,2}
{170,*,3}
{171,*,0}
{172,*,7}
{173,*,8}
{174,*,14}
{180,*,10}
{190,*,7}
{200,*,12}
*/
