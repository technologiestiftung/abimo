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
#include "effectivenessunsealed.h"
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
    counters({0, 0, 0, 0L, 0L, 0L}),
    weiter(true)
{
    config = new Config();
}

void Calculation::stop()
{
    weiter = false;
}

Counters Calculation::getCounters()
{
    return counters;
}

QString Calculation::getError()
{
    return error;
}

// =============================================================================
// Diese Funktion importiert die Datensaetze aus der DBASE-Datei FileName in das DA Feld ein
// (GWD-Daten). Parameter: out-file Rueckgabewert: BOOL TRUE, wenn das Einlesen der Datei
// erfolgreich war.
// =============================================================================
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
    counters.protcount = 0L;
    counters.keineFlaechenAngegeben = 0L;
    counters.nutzungIstNull = 0L;

    // first entry into protocol
    DbaseWriter writer(fileOut, initValues);

    // get the number of rows in the input data ?
    counters.totalRecRead = dbReader.getNumberOfRecords();

    // loop over all block partial areas (records) of input data
    for (k = 0; k < counters.totalRecRead; k++) {

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
                counters.protcount++;
                counters.keineFlaechenAngegeben++;
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
            counters.nutzungIstNull++;

        }

        /* cls_2: Hier koennten falls gewuenscht die Flaechen dokumentiert werden,
           deren NUTZUNG=NULL (siehe auch cls_3)
        */

        emit processSignal((int)((float) k / (float) counters.totalRecRead * 50.0), "Berechne");
    }

    counters.totalRecWrite = index;

    emit processSignal(50, "Schreibe Ergebnisse.");

    if (!writer.write()) {
        protokollStream << "Error: "+ writer.getError() +"\r\n";
        error = "Fehler beim Schreiben der Ergebnisse.\n" + writer.getError();
        return false;
    }

    return true;
}

// =============================================================================
// FIXME:
// =============================================================================
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

    if (ptrDA.NUT != Usage::waterbody_G)
    {
        /* pot. Aufstiegshoehe TAS = FLUR - mittl. Durchwurzelungstiefe TWS */
        TAS = ptrDA.FLW - config->getTWS(ptrDA.ERT, ptrDA.NUT);

        /* Feldkapazitaet */
        /* cls_6b: der Fall der mit NULL belegten FELD_30 und FELD_150 Werte
           wird hier im erten Fall behandelt - ich erwarte dann den Wert 0 */
        ptrDA.nFK = PDR::estimateWaterHoldingCapacity(f30, f150, ptrDA.NUT == Usage::forested_W);

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
        counters.totalBERtoZeroForced++;
        ptrDA.BER = 0;
    }
}

void Calculation::setUsageYieldIrrigation(int usage, int type, QString code)
{
    UsageResult result;

    result = config->getUsageResult(usage, type, code);

    if (result.tupleIndex < 0) {
        protokollStream << result.message;
        qDebug() << result.message;
       abort();
    }

    if (!result.message.isEmpty()) {
        protokollStream << result.message;
        counters.protcount++;
    }

    ptrDA.setUsageYieldIrrigation(config->getUsageTuple(result.tupleIndex));
}

// =============================================================================
// FIXME:
// =============================================================================
void Calculation::getKLIMA(int bez, QString code)
{
    // Effektivitaetsparameter
    float bag;

    // ep = potential evaporation
    float ep;

    // prepcipitation at ground level
    float p;

    // ratio of precipitation to potential evaporation
    float x;

    // ratio of real evaporation to potential evaporation
    float y;

    // real evapotranspiration
    float etr;

    /*
     * spaeter zeizusaetzliche Parameter Hier ;
     * ptrDA.P1 = p1;
     * * ptrDA.PS = ps;
     */
    ptrDA.P1 = regenja;
    ptrDA.P1S = regenso;

    // parameter for the city districts
    if (ptrDA.NUT == Usage::waterbody_G)
    {
        ptrDA.ETP = initValueOrReportedDefaultValue(
            bez, code, initValues.hashEG, 775, "EG"
        );
    }
    else
    {
        ptrDA.ETP = initValueOrReportedDefaultValue(
            bez, code, initValues.hashETP, 660, "ETP"
        );

        ptrDA.ETPS = initValueOrReportedDefaultValue(
            bez, code, initValues.hashETPS, 530, "ETPS"
        );
    }

    // declaration potential evaporation ep and precipitation p
    ep = (float) ptrDA.ETP; /* Korrektur mit 1.1 gestrichen */
    p = (float) ptrDA.P1 * initValues.getNiedKorrF(); /* ptrDA.KF */

    /*
     * Berechnung der Abfluesse RDV und R1V bis R4V fuer versiegelte
     * Teilflaechen und unterschiedliche Bagrovwerte ND und N1 bis N4
     */

    // ratio precipitation to potential evaporation
    x = p / ep;

    Bagrov bagrov;

    /* Berechnung des Abflusses RxV fuer versiegelte Teilflaechen mittels
       Umrechnung potentieller Verdunstungen ep zu realen über Umrechnungsfaktor y und
       subtrahiert von Niederschlag p */

    RDV = p - bagrov.nbagro(initValues.getBagdach(), x) * ep;
    R1V = p - bagrov.nbagro(initValues.getBagbel1(), x) * ep;
    R2V = p - bagrov.nbagro(initValues.getBagbel2(), x) * ep;
    R3V = p - bagrov.nbagro(initValues.getBagbel3(), x) * ep;
    R4V = p - bagrov.nbagro(initValues.getBagbel4(), x) * ep;

    // Calculate runoff RUV for unsealed partial surfaces
    if (ptrDA.NUT == Usage::waterbody_G)
    {
        RUV = p - ep;
    }
    else
    {
        // Determine effectiveness parameter bag for unsealed surfaces
        bag = EffectivenessUnsealed::getNUV(ptrDA); /* Modul Raster abgespeckt */

        if (ptrDA.P1S > 0 && ptrDA.ETPS > 0) {
            bag *= getSummerModificationFactor(
                (float) (ptrDA.P1S + ptrDA.BER + ptrDA.KR) / ptrDA.ETPS
            );
        }

        // Calculate the x-factor of bagrov relation: x = (P + KR + BER)/ETP
        // Then get the y-factor: y = fbag(n, x)
        y = bagrov.nbagro(bag, (p + ptrDA.KR + ptrDA.BER) / ep);

        // Get the real evapotransporation using estimated y-factor
        etr = y * ep;

        if (TAS < 0) {
            etr += (ep - y * ep) * (float) exp(ptrDA.FLW / TAS);
        }

        RUV = p - etr;
    }
}

float Calculation::initValueOrReportedDefaultValue(
    int bez, QString code, QHash<int, int> &hash, int defaultValue, QString name
)
{
    if (hash.contains(bez)) {
        //take from xml
        return hash.value(bez);
    }

    //default
    float result = hash.contains(0) ? hash.value(0) : defaultValue;

    QString bezString;
    bezString.setNum(bez);

    QString string;
    string.setNum(result);

    protokollStream << "\r\n" + name + " unbekannt fuer " + code +
        " von Bezirk " + bezString + "\r\n" + name +
        "=" + string + " angenommen\r\n";
    counters.protcount++;

    return result;
}

// =============================================================================
// Get factor to be applied for "summer"
// =============================================================================
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
