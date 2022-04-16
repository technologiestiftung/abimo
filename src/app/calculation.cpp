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
#include "dbaseReader.h"
#include "dbaseWriter.h"
#include "helpers.h"
#include "initvalues.h"
#include "pdr.h"

// parameter values x1, x2, x3, x4 and x5 (one column each)
// for calculating the effectiveness parameter n for unsealed surfaces
const float Calculation::EKA[]= {
    0.04176F, -0.647F , 0.218F  ,  0.01472F, 0.0002089F,
    0.04594F, -0.314F , 0.417F  ,  0.02463F, 0.0001143F,
    0.05177F, -0.010F , 0.596F  ,  0.02656F, 0.0002786F,
    0.05693F,  0.033F , 0.676F  ,  0.0279F , 0.00035F  ,
    0.06162F,  0.176F , 0.773F  ,  0.02809F, 0.0004695F,
    0.06962F,  0.24F  , 0.904F  ,  0.02562F, 0.0007149F,
    0.0796F ,  0.31F  , 1.039F  ,  0.0288F , 0.0008696F,
    0.07998F,  0.7603F, 1.2F    ,  0.0471F , 0.000293F ,
    0.08762F,  1.019F , 1.373F  ,  0.04099F, 0.0014141F,
    0.11833F,  1.1334F, 1.95F   ,  0.0525F , 0.00125F  ,
    0.155F  ,  1.5F   , 2.64999F,  0.0725F , 0.001249F ,
    0.20041F,  2.0918F, 3.69999F,  0.08F   , 0.001999F ,
    0.33895F,  3.721F , 6.69999F, -0.07F   , 0.013F
};

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

Calculation::Calculation(DbaseReader & dbR, InitValues & init, QTextStream & protoStream):
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

    // get precipitation correction factor from config.xml containing the initial values
    niedKorrFaktor = initValues.getNiedKorrF();

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
        fillRecord(k, record, debug);

        // NUTZUNG = integer representing the type of area usage for each block partial area
        if (record.NUTZUNG != 0) {

            // get identifier number 'CODE' for each block partial area
            QString code = record.CODE;

            // get precipitation for entire year 'regenja' and for only summer season 'regenso'
            regenja = record.REGENJA; /* Jetzt regenja,-so OK */
            regenso = record.REGENSO;

            // get depth to groundwater table 'FLUR'
            ptrDA.FLW = record.FLUR;

            // get structure type 'TYP', field capacity [%] for 0-30cm 'FELD_30' and 0-150cm 'FELD_150' below ground level
            getNUTZ(record.NUTZUNG, record.TYP, record.FELD_30, record.FELD_150, code);

            /* cls_6a: an dieser Stelle muss garantiert werden, dass f30 und f150
               als Parameter von getNutz einen definierten Wert erhalten und zwar 0.

               FIXED: alle Werte sind definiert... wenn keine 0, sondern nichts bzw. Leerzeichen
               angegeben wurden, wird nun eine 0 eingesetzt
               aber eigentlich war das auch schon so ... ???
            */

            // Bagrov-calculation for sealed surfaces
            getKLIMA(record.BEZIRK, code);

            // get share of roof area [%] 'PROBAU'
            vgd = record.PROBAU / 100.0F;
          
            // get share of other sealed areas (e.g. Hofflaechen) and calculate total sealed area
            vgb = record.PROVGU / 100.0F;
            ptrDA.VER = (int)round((vgd * 100) + (vgb * 100));
            
            // get share of sealed road area
            vgs = record.VGSTRASSE / 100.0F;
          
            // get degree of canalization for roof / other sealed areas / sealed roads
            kd = record.KAN_BEB / 100.0F;
            kb = record.KAN_VGU / 100.0F;
            ks = record.KAN_STR / 100.0F;
          
            // get share of each pavement class for surfaces except roads of block area
            bl1 = record.BELAG1 / 100.0F;
            bl2 = record.BELAG2 / 100.0F;
            bl3 = record.BELAG3 / 100.0F;
            bl4 = record.BELAG4 / 100.0F;
          
            // get share of each pavement class for roads of block area
            bls1 = record.STR_BELAG1 / 100.0F;
            bls2 = record.STR_BELAG2 / 100.0F;
            bls3 = record.STR_BELAG3 / 100.0F;
            bls4 = record.STR_BELAG4 / 100.0F;
          
            fb = record.FLGES;
            fs = (dbReader.getRecord(k, "STR_FLGES")).toFloat();
            
            // if sum of total building development area and roads area is inconsiderably small
            // it is assumed, that the area is unknown and 100 % building development area will be given by default
            if (fb + fs < 0.0001)
            {
                //*protokollStream << "\r\nDie Flaeche des Elements " + code + " ist 0 \r\nund wird automatisch auf 100 gesetzt\r\n";
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
            ptrDA.ROW = (int)round(row);
            
            // calculate volume 'rowvol' from runoff
            ROWVOL = row * 3.171F * (fb + fs) / 100000.0F;     // qcm/s
            
            // calculate infiltration rate 'ri' for entire block partial area
            ri = (ri1 + ri2 + ri3 + ri4 + rid + riuvs + riuv); // mm/a
            ptrDA.RI = (int)round(ri);
            
            // calculate volume 'rivol' from infiltration rate
            RIVOL = ri * 3.171F * (fb + fs) / 100000.0F;       // qcm/s
            
            // calculate total system losses 'r' due to runoff and infiltration for entire block partial area
            r = row + ri;
            ptrDA.R = (int)round(r);
            
            // calculate volume of system losses 'rvol'due to runoff and infiltration
            RVOL = ROWVOL + RIVOL;

            // calculate total area of building development area as well as roads area
            float flaeche1 = fb + fs;
// cls_5b:
            // calculate evaporation 'verdunst' by subtracting the sum of runoff and infiltration 'r'
            // from precipitation of entire year 'regenja' multiplied by correction factor 'niedKorrFaktor'
            float verdunst = (regenja * niedKorrFaktor) - r;

            // write the calculated variables into respective fields
            writer.addRecord();
            writer.setRecordField("CODE", code);
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
    /* globale Groessen fuer den aktuellen Record */
    float kr;  /* mittlere pot. kapillare Aufstiegsrate d. Sommerhalbjahres */

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
    switch (usage)
    {
    case 10:
    case 21:
    case 22:
    case 23:
    case 30:
        switch (type)
        {
        // cases for agricultural land use (NUT = 'L') of different yield power (ERT)

        case 29:
            ptrDA.setUsageYieldIrrigation('L', 30); break;

        case  1:
        case  2:
        case  7:
        case  8:
        case 11:
        case 33:
        case 38:
        case 39:
            ptrDA.setUsageYieldIrrigation('L', 35); break;

        case  4:
        case  5:
        case  6:
        case  9:
        case 10:
        case 26:
            ptrDA.setUsageYieldIrrigation('L', 40); break;

        case  3:
        case 21:
        case 71:
            ptrDA.setUsageYieldIrrigation('L', 45); break;

        // cls_4: Baustrukturtypen 73 und 74 neu eingefuehrt - werden behandelt wie 72
        case 72:
        case 73:
        case 74:
            ptrDA.setUsageYieldIrrigation('L', 50); break;

        // cases for gardening (NUT = 'K') of yield power (ERT) and certain irrigation (BER)
        case 22:
        case 23:
            ptrDA.setUsageYieldIrrigation('K', 40, 75); break;

        case 24:
            ptrDA.setUsageYieldIrrigation('L', 55, 75); break;

        case 25:
            ptrDA.setUsageYieldIrrigation('K', 40, 75); break;

        default:
            logNotDefined(code, 72);
            ptrDA.setUsageYieldIrrigation('L', 50);
            break;
        }
        break;

    case 40:
        switch (type)
        {
        case 30:
            ptrDA.setUsageYieldIrrigation('L', 35); break;
        case 31:
            ptrDA.setUsageYieldIrrigation('L', 30); break;
        default:
            logNotDefined(code, 31);
            ptrDA.setUsageYieldIrrigation('L', 30);
            break;
        }
        break;

    case 50:
        switch (type)
        {
        case 42:
        case 43:
            ptrDA.setUsageYieldIrrigation('L', 35); break;

        case 28:
        case 41:
        case 45:
            ptrDA.setUsageYieldIrrigation('L', 40); break;

        case 12:
        case 47:
        case 51:
        case 60:
            ptrDA.setUsageYieldIrrigation('L', 45); break;

        case 13:
        case 14:
            ptrDA.setUsageYieldIrrigation('L', 50); break;

        case 44:
        case 49:
        case 50:
            ptrDA.setUsageYieldIrrigation('L', 45, 50); break;

        case 46:
            ptrDA.setUsageYieldIrrigation('L', 50, 50); break;

        default:
            logNotDefined(code, 60);
            ptrDA.setUsageYieldIrrigation('L', 45);
            break;
        }
        break;

    case 60:
        ptrDA.setUsageYieldIrrigation('L', 45); break;

    case 70:
        switch (type)
        {
        case 59:
            ptrDA.setUsageYieldIrrigation('K', 40, 75); break;
        default:
            logNotDefined(code, 59);
            ptrDA.setUsageYieldIrrigation('K', 40, 75);
            break;
        }
        break;

    case 80:
        switch (type)
        {
        case 99:
            ptrDA.setUsageYieldIrrigation('L', 10); break;

        case 92:
            ptrDA.setUsageYieldIrrigation('L', 25); break;

        case 93:
        case 94:
            ptrDA.setUsageYieldIrrigation('L', 30); break;

        case 91:
            ptrDA.setUsageYieldIrrigation('L', 40); break;

        default:
            logNotDefined(code, 99);
            ptrDA.setUsageYieldIrrigation('L', 10);
            break;
        }
        break;

    case 90:
        switch (type)
        {
        case 98:
            ptrDA.setUsageYieldIrrigation('D', 1); break;
        default:
            logNotDefined(code, 98);
            ptrDA.setUsageYieldIrrigation('D', 1);
            break;
        }
        break;

    case 100:
        switch (type)
        {
        case 55:
            ptrDA.setUsageYieldIrrigation('W'); break;
        default:
            logNotDefined(code, 55);
            ptrDA.setUsageYieldIrrigation('W');
            break;
        }
        break;

    case 101:
        ptrDA.setUsageYieldIrrigation('W'); break;

    case 102:
        ptrDA.setUsageYieldIrrigation('L', 60); break;

    case 110:
        ptrDA.setUsageYieldIrrigation('G'); break;

    case 121:
        ptrDA.setUsageYieldIrrigation('L', 40); break;

    case 122:
        ptrDA.setUsageYieldIrrigation('L', 35); break;

    case 130:
        ptrDA.setUsageYieldIrrigation('L', 50, 50); break;

    case 140:
        ptrDA.setUsageYieldIrrigation('L', 50); break;

    case 150:
        ptrDA.setUsageYieldIrrigation('L', 50, 100); break;

    case 160:
    case 161:
    case 162:
        ptrDA.setUsageYieldIrrigation('K', 40, 75); break;

    case 170:
        ptrDA.setUsageYieldIrrigation('L', 10); break;

    case 171:
        ptrDA.setUsageYieldIrrigation('D', 1); break;

    case 172:
    case 190:
        ptrDA.setUsageYieldIrrigation('L', 40); break;

    case 173:
        ptrDA.setUsageYieldIrrigation('L', 45); break;

    case 174:
        ptrDA.setUsageYieldIrrigation('L', 60); break;

    case 180:
        ptrDA.setUsageYieldIrrigation('L', 50); break;

    case 200:
        ptrDA.setUsageYieldIrrigation('L', 50, 50); break;

    default:
        /*
           logNotDefined(code, 200);
           ptrDA.setUsageYieldIrrigation('L', 50, 50);
           cls_3: dies ist nicht korrekt, da die Nutzung und nicht der Nutzungstyp im switch liegt
           und ein NULL in NUTZUNG hoffentlich immer zu nutzung=0 fuehrt, wenn oben
           int nutzung = dbReader.getRecord(k, "NUTZUNG").toInt();
           aufgerufen wird (siehe auch cls_2) -
           deshalb folgende Fehlermeldung
        */
        protokollStream << "\r\nDiese  Meldung sollte nie erscheinen: \r\nNutzung nicht definiert fuer Element " + code + "\r\n";
        break;
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
    p = (float) ptrDA.P1 * niedKorrFaktor; /* ptrDA.KF */

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
        n = getNUV(ptrDA); /* Modul Raster abgespeckt */

        if (ptrDA.P1S > 0 && ptrDA.ETPS > 0)
        {
            zw = (float) (ptrDA.P1S + ptrDA.BER + ptrDA.KR) / ptrDA.ETPS;
            bag = getF(zw) * n; /* Modifikation des Parameters durch "Sommer" */
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

/*
 =======================================================================================================================
    FIXME:
 =======================================================================================================================
 */
float Calculation::getF(float wa)
{
    int i, anz;

    const float watab[] =
    {
        0.45F, 0.50F, 0.55F, 0.60F, 0.65F, 0.70F, 0.75F,
        0.80F, 0.85F, 0.90F, 0.95F, 1.00F, 1.05F, 1.10F
    };

    const float Ftab[] =
    {
        0.65F, 0.75F, 0.82F, 0.90F, 1.00F, 1.06F, 1.15F,
        1.22F, 1.30F, 1.38F, 1.47F, 1.55F, 1.63F, 1.70F
    };

    anz = 14;

    if (wa <= watab[0]) return(Ftab[0]);
    if (wa >= watab[anz - 1]) return(Ftab[anz - 1]);

    for (i = 1; i < anz; i++)
        if (wa <= watab[i]) return((Ftab[i - 1] + Ftab[i]) / 2);

    return 0;
}

/*
 =======================================================================================================================
    FIXME:
 =======================================================================================================================
 */
float Calculation::getG02(int nFK)
{
    const float G02tab [] = {
        0.0F,   0.0F,  0.0F,  0.0F,  0.3F,  0.8F,  1.4F,  2.4F,  3.7F,  5.0F,
        6.3F,   7.7F,  9.3F, 11.0F, 12.4F, 14.7F, 17.4F, 21.0F, 26.0F, 32.0F,
        39.4F, 44.7F, 48.0F, 50.7F, 52.7F, 54.0F, 55.0F, 55.0F, 55.0F, 55.0F, 55.0F
    };

    return G02tab[nFK];
}

/**
 =======================================================================================================================
    g e t N U V ( Pointer auf aktuellen DataRecord) Berechnung eines Records (abgespecktes Raster)
 =======================================================================================================================
 */
float Calculation::getNUV(PDR &B) /* DataRecord_t *B) */
{
    int K;
    float G020, BAG0;

    G020 = getG02((int) (B.nFK + 0.5));

    switch (B.NUT)
    {
    case 'W':
        if (G020 <= 10.0)
            BAG0 = 3.0F;
        else if (G020 <= 25.0)
            BAG0 = 4.0F;
        else
            BAG0 = 8.0F;
        break;

    default:
        K = (int) (B.ERT / 5);
        if (B.ERT > 49) K = (int) (B.ERT / 10 + 5);
        if (K <= 0) K = 1;
        if (K >= 4) K = K - 1;
        if (K > 13) K = 13;
        K = 5 * K - 2;
        BAG0 = EKA[K - 1] + EKA[K] * G020 + EKA[K + 1] * G020 * G020;
        if ((BAG0 >= 2.0) && (B.ERT < 60)) BAG0 = EKA[K - 3] * G020 + EKA[K - 2];
        if ((G020 >= 20.0) && (B.ERT >= 60)) BAG0 = EKA[K - 3] * G020 + EKA[K - 2];

        if (B.BER > 0 && (B.P1S == 0 && B.ETPS == 0)) /* Modifikation, wenn keine Sommerwerte */
            BAG0 = BAG0 * (0.9985F + 0.00284F * B.BER - 0.00000379762F * B.BER * B.BER);
        break;
    }

    return BAG0;
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

void Calculation::fillRecord(int k, abimoRecord& record, bool debug)
{
    record.BELAG1 = dbReader.getRecord(k, "BELAG1").toFloat();
    record.BELAG2 = dbReader.getRecord(k, "BELAG2").toFloat();
    record.BELAG3 = dbReader.getRecord(k, "BELAG3").toFloat();
    record.BELAG4 = dbReader.getRecord(k, "BELAG4").toFloat();
    record.BEZIRK = dbReader.getRecord(k, "BEZIRK").toInt();
    record.CODE = dbReader.getRecord(k, "CODE");
    record.FELD_150 = dbReader.getRecord(k, "FELD_150").toInt();
    record.FELD_30 = dbReader.getRecord(k, "FELD_30").toInt();
    record.FLGES = dbReader.getRecord(k, "FLGES").toFloat();
    record.FLUR = dbReader.getRecord(k, "FLUR").toFloat();
    record.KAN_BEB = dbReader.getRecord(k, "KAN_BEB").toFloat();
    record.KAN_STR = dbReader.getRecord(k, "KAN_STR").toFloat();
    record.KAN_VGU = dbReader.getRecord(k, "KAN_VGU").toFloat();
    record.NUTZUNG = Helpers::stringToInt(
        dbReader.getRecord(k, "NUTZUNG"),
        QString("k: %1, NUTZUNG = ").arg(QString::number(k)),
        debug
    );
    record.PROBAU = Helpers::stringToFloat(
        dbReader.getRecord(k, "PROBAU"),
        QString("k: %1, PROBAU = ").arg(QString::number(k)),
        debug
    );
    record.PROVGU = dbReader.getRecord(k, "PROVGU").toFloat();
    record.REGENJA = dbReader.getRecord(k, "REGENJA").toInt();
    record.REGENSO = dbReader.getRecord(k, "REGENSO").toInt();
    record.STR_BELAG1 = dbReader.getRecord(k, "STR_BELAG1").toFloat();
    record.STR_BELAG2 = dbReader.getRecord(k, "STR_BELAG2").toFloat();
    record.STR_BELAG3 = dbReader.getRecord(k, "STR_BELAG3").toFloat();
    record.STR_BELAG4 = dbReader.getRecord(k, "STR_BELAG4").toFloat();
    record.TYP = dbReader.getRecord(k, "TYP").toInt();
    record.VGSTRASSE = dbReader.getRecord(k, "VGSTRASSE").toFloat();
}
