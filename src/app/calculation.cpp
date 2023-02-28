// ***************************************************************************
// * For copyright information please see COPYRIGHT in the base directory
// * of this repository (https://github.com/KWB-R/abimo).
// ***************************************************************************

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

// Macro to calculate the number of elements in an array
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

// Potential rate of ascent (column labels for matrix
// meanPotentialCapillaryRiseRateSummer)
// old: iTAS
const float Calculation::POTENTIAL_RATES_OF_ASCENT[] = {
    0.1F, 0.2F, 0.3F, 0.4F, 0.5F, 0.6F, 0.7F, 0.8F,
    0.9F, 1.0F, 1.2F, 1.4F, 1.7F, 2.0F, 2.3F
};

// soil type unknown - default soil type used in the following: sand

// Usable field capacity (row labels for matrix
// meanPotentialCapillaryRiseRateSummer)
// old: inFK_S
const float Calculation::USABLE_FIELD_CAPACITIES[] = {
    8.0F, 9.0F, 14.0F, 14.5F, 15.5F, 17.0F, 20.5F
};

// Mean potential capillary rise rate kr [mm/d] of a summer season depending on:
// - Potential rate of ascent (one column each) and
// - Usable field capacity (one row each)
// old: ijkr_S
const float Calculation::MEAN_POTENTIAL_CAPILLARY_RISE_RATES_SUMMER[] = {
    7.0F, 6.0F, 5.0F, 1.5F, 0.5F, 0.2F, 0.1F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 5.0F, 3.0F, 1.2F, 0.5F, 0.2F, 0.1F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 3.0F, 1.5F, 0.7F, 0.3F, 0.15F, 0.1F, 0.0F, 0.0F, 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 3.0F, 2.0F, 1.0F, 0.7F, 0.4F, 0.15F, 0.1F, 0.0F, 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 4.5F, 2.5F, 1.5F, 0.7F, 0.4F, 0.15F, 0.1F, 0.0F, 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 5.0F, 5.0F, 3.5F, 2.0F, 1.5F, 0.8F, 0.3F, 0.1F, 0.05F, 0.0F, 0.0F,
    7.0F, 7.0F, 6.0F, 6.0F, 6.0F, 5.0F, 5.0F, 5.0F, 3.0F, 2.0F, 1.0F, 0.5F, 0.15F, 0.0F, 0.0F
};

Calculation::Calculation(
        DbaseReader &dbaseReader,
        InitValues &initValues,
        QTextStream &protocolStream
):
    initValues(initValues),
    protocolStream(protocolStream),
    dbReader(dbaseReader),
    precipitationYear(0), // old: regenja
    precipitationSummer(0), // old: regenso
    bagrovRoof(0), // old: RDV
    bagrovSurfaceClass1(0), // old: R1V
    bagrovSurfaceClass2(0), // old: R2V
    bagrovSurfaceClass3(0), // old: R3V
    bagrovSurfaceClass4(0), // old: R4V
    unsealedSurfaceRunoff(0), // old: RUV
    rainwaterRunoff(0), // old: ROWVOL
    totalSubsurfaceFlow(0), // old: RIVOL
    totalRunoff(0), // old: RVOL
    potentialCapillaryRise(0), // old: TAS
    n_POTENTIAL_RATES_OF_ASCENT(ARRAY_SIZE(POTENTIAL_RATES_OF_ASCENT)), // old: lenTAS
    n_USABLE_FIELD_CAPACITIES(ARRAY_SIZE(USABLE_FIELD_CAPACITIES)), // old: lenS
    counters({0, 0, 0, 0L, 0L, 0L}),
    continueProcessing(true) // old: weiter
{
    config = new Config();
}

void Calculation::stopProcessing()
{
    continueProcessing = false;
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
// Diese Funktion importiert die Datensaetze aus der DBASE-Datei FileName in das
// DA Feld ein (GWD-Daten).
// Parameter: outputFile: Name der Ausgabedatei
//            debug: whether or not to show debug messages
// Rueckgabewert: BOOL. TRUE, wenn das Einlesen der Datei erfolgreich war.
// =============================================================================
bool Calculation::calculate(QString outputFile, bool debug)
{
    // Current Abimo record (represents one row of the input dbf file)
    abimoRecord record;

    // Number of processed records
    int index = 0;
    
    // Versiegelungsgrad Dachflaechen / sonst. versiegelte Flaechen / Strassen
    // degree of sealing of roof surfaces / other sealed surfaces / roads
    float imperviousnessRoof; // old: vgd
    float imperviousnessOther; // old: vgb
    float imperviousnessRoad; // old: vgs

    // Kanalisierungsgrad Dachflaechen / sonst. versiegelte Flaechen / Strassen
    // degree of canalization for roof surfaces / other sealed surfaces / roads
    float connectednessRoof; // old: kd
    float connectednessOther; // old: kb
    float connectednessRoad; // old: ks
    
    // Anteil der jeweiligen Belagsklasse
    // share of respective pavement class
    float shareOfSurfaceClass1; // old: bl1
    float shareOfSurfaceClass2; // old: bl2
    float shareOfSurfaceClass3; // old: bl3
    float shareOfSurfaceClass4; // old: bl4

    // Anteil der jeweiligen Strassenbelagsklasse
    // share of respective road pavement class
    float shareOfRoadClass1; // old: bls1
    float shareOfRoadClass2; // old: bls2
    float shareOfRoadClass3; // old: bls3
    float shareOfRoadClass4; // old: bls4

    // Gesamtflaeche Bebauung / Strasse
    // total area of building development / road
    float totalAreaBuildings; // old: fb
    float totalAreaRoads; // old: fs

    // Verhaeltnis Bebauungsflaeche / Strassenflaeche zu Gesamtflaeche
    // (ant = Anteil)
    // share of building development area / road area to total area
    float areaShareBuildings; // old: fbant
    float areaShareRoads; // old: fsant
    
    // Abflussvariablen der versiegelten Flaechen
    // runoff variables of sealed surfaces
    float runoffSealedSurface1; // old: row1
    float runoffSealedSurface2; // old: row2
    float runoffSealedSurface3; // old: row3
    float runoffSealedSurface4; // old: row4
    
    // Infiltrationsvariablen der versiegelten Flaechen
    // infiltration variables of sealed surfaces
    float infiltrationFromSealedSurface1; // old: ri1
    float infiltrationFromSealedSurface2; // old: ri2
    float infiltrationFromSealedSurface3; // old: ri3
    float infiltrationFromSealedSurface4; // old: ri4
    
    // Abfluss- / Infiltrationsvariablen der Dachflaechen
    // runoff- / infiltration variables of roof surfaces
    float runoffRoofs; // old: rowd
    float infiltrationFromRoofs; // old: rid
    
    // Abfluss- / Infiltrationsvariablen unversiegelter Strassenflaechen
    // runoff- / infiltration variables of unsealed road surfaces
    float runoffPerviousRoads; // old: rowuvs
    float infiltrationFromPerviousRoads; // old: riuvs
    
    // Infiltration unversiegelter Flaechen
    // infiltration of unsealed areas
    float infiltrationFromPerviousSurfaces; // old: riuv
    
    // float-Zwischenwerte
    // float intermediate values
    float r, ri, row;

    // count protocol entries
    counters.recordsProtocol = 0L;
    counters.noAreaGiven = 0L;
    counters.noUsageGiven = 0L;

    // first entry into protocol
    DbaseWriter writer(outputFile, initValues);

    // get the number of rows in the input data
    counters.recordsRead = dbReader.getNumberOfRecords();

    // loop over all block partial areas (= records/rows of input data)
    int k;
    for (k = 0; k < counters.recordsRead; k++) {

        if (!continueProcessing) {
            protocolStream << "Berechnungen abgebrochen.\r\n";
            return true;
        }

        resultRecord.wIndex = index;

        // Fill record with data from row k
        dbReader.fillRecord(k, record, debug);

        // NUTZUNG = integer representing the type of area usage for each block
        // partial area
        if (record.usage != 0) {

            // CODE: unique identifier for each block partial area

            // precipitation for entire year and for summer season only
            precipitationYear = record.precipitationYear;
            precipitationSummer = record.precipitationSummer;

            // depth to groundwater table 'FLUR'
            resultRecord.depthToWaterTable = record.depthToWaterTable;

            getUsage(
                record.usage,
                // structure type
                record.type,
                // field capacity [%] for 0-30cm below ground level
                record.fieldCapacity_30,
                // field capacity [%] for 0-150cm below ground level
                record.fieldCapacity_150,
                record.code
            );

            // cls_6a: an dieser Stelle muss garantiert werden, dass f30 und
            // f150 als Parameter von getNUTZ einen definierten Wert erhalten
            // und zwar 0.
            // FIXED: alle Werte sind definiert... wenn keine 0, sondern nichts
            // bzw. Leerzeichen angegeben wurden, wird nun eine 0 eingesetzt
            // aber eigentlich war das auch schon so ... ???

            // Bagrov-calculation for sealed surfaces
            getClimaticConditions(record.district, record.code);

            // share of roof area [%] 'PROBAU'
            imperviousnessRoof = record.imperviousnessRoof;
          
            // share of other sealed areas (e.g. Hofflaechen) and calculate
            // total sealed area
            imperviousnessOther = record.imperviousnessOther;
            resultRecord.imperviousness = INT_ROUND(
                imperviousnessRoof * 100 +
                imperviousnessOther * 100
            );
            
            // share of sealed road area
            imperviousnessRoad = record.imperviousnessRoad;
          
            // degree of canalization for roof / other sealed areas /
            // sealed roads
            connectednessRoof = record.connectednessRoof;
            connectednessOther = record.connectednessOther;
            connectednessRoad = record.connectednessRoad;
          
            // share of each pavement class for surfaces except roads of block
            // area
            shareOfSurfaceClass1 = record.shareOfSurfaceClass1;
            shareOfSurfaceClass2 = record.shareOfSurfaceClass2;
            shareOfSurfaceClass3 = record.shareOfSurfaceClass3;
            shareOfSurfaceClass4 = record.shareOfSurfaceClass4;
          
            // share of each pavement class for roads of block area
            shareOfRoadClass1 = record.shareOfRoadClass1;
            shareOfRoadClass2 = record.shareOfRoadClass2;
            shareOfRoadClass3 = record.shareOfRoadClass3;
            shareOfRoadClass4 = record.shareOfRoadClass4;
          
            totalAreaBuildings = record.totalAreaBuildings;
            totalAreaRoads = record.totalAreaRoads;
            
            // if sum of total building development area and road area is
            // inconsiderably small it is assumed, that the area is unknown and
            // 100 % building development area will be given by default
            if (totalAreaBuildings + totalAreaRoads < 0.0001) {
                // *protokollStream << "\r\nDie Flaeche des Elements " +
                // record.CODE + " ist 0 \r\nund wird automatisch auf 100 gesetzt\r\n";
                counters.recordsProtocol++;
                counters.noAreaGiven++;
                totalAreaBuildings = 100.0F;
            }

            float totalArea = totalAreaBuildings + totalAreaRoads;

            // Verhaeltnis Bebauungsflaeche zu Gesamtflaeche
            // ratio of building development area to total area
            areaShareBuildings = totalAreaBuildings / totalArea;
            
            // Verhaeltnis Strassenflaeche zu Gesamtflaeche
            // ratio of roads area to total area
            areaShareRoads = totalAreaRoads / totalArea;

            // Runoff for sealed surfaces
            // cls_1: Fehler a:
            //   rowd = (1.0F - initValues.getInfdach()) * vgd * kb * fbant * RDV;
            //   richtige Zeile folgt (kb ----> kd)
            
            //  Legende der Abflussberechnung der 4 Belagsklassen bzw. Dachklasse:
            //    rowd / rowx: Abfluss Dachflaeche / Abfluss Belagsflaeche x
            //    infdach / infbelx: Infiltrationsparameter Dachfl. / Belagsfl. x
            //    belx: Anteil Belagsklasse x
            //    blsx: Anteil Strassenbelagsklasse x
            //    vgd / vgb: Anteil versiegelte Dachfl. / sonstige versiegelte Flaeche zu Gesamtblockteilflaeche
            //    kd / kb / ks: Grad der Kanalisierung Dach / sonst. vers. Fl. / Strassenflaechen
            //    fbant / fsant: ?
            //    RDV / RxV: Gesamtabfluss versiegelte Flaeche

            runoffRoofs = (1.0F - initValues.getInfiltrationFactorRoof()) *
                imperviousnessRoof *
                connectednessRoof *
                areaShareBuildings *
                bagrovRoof;

            runoffSealedSurface1 = (1.0F - initValues.getInfiltrationFactorSurfaceClass1()) * (
                shareOfSurfaceClass1 * connectednessOther * imperviousnessOther * areaShareBuildings +
                shareOfRoadClass1 * connectednessRoad * imperviousnessRoad * areaShareRoads
            ) * bagrovSurfaceClass1;

            runoffSealedSurface2 = (1.0F - initValues.getInfiltrationFactorSurfaceClass2()) * (
                shareOfSurfaceClass2 * connectednessOther * imperviousnessOther * areaShareBuildings +
                shareOfRoadClass2 * connectednessRoad * imperviousnessRoad * areaShareRoads
            ) * bagrovSurfaceClass2;

            runoffSealedSurface3 = (1.0F - initValues.getInfiltrationFactorSurfaceClass3()) * (
                shareOfSurfaceClass3 * connectednessOther * imperviousnessOther * areaShareBuildings +
                shareOfRoadClass3 * connectednessRoad * imperviousnessRoad * areaShareRoads
            ) * bagrovSurfaceClass3;

            runoffSealedSurface4 = (1.0F - initValues.getInfiltrationFactorSurfaceClass4()) * (
                shareOfSurfaceClass4 * connectednessOther * imperviousnessOther * areaShareBuildings +
            shareOfRoadClass4 * connectednessRoad * imperviousnessRoad * areaShareRoads
                        ) * bagrovSurfaceClass4;

            // Infiltration for sealed surfaces
            infiltrationFromRoofs = (1 - connectednessRoof) * imperviousnessRoof * areaShareBuildings * bagrovRoof;

            infiltrationFromSealedSurface1 = (
                shareOfSurfaceClass1 * imperviousnessOther * areaShareBuildings +
                shareOfRoadClass1 * imperviousnessRoad * areaShareRoads
            ) * bagrovSurfaceClass1 - runoffSealedSurface1;

            infiltrationFromSealedSurface2 = (
                shareOfSurfaceClass2 * imperviousnessOther * areaShareBuildings +
                shareOfRoadClass2 * imperviousnessRoad * areaShareRoads
            ) * bagrovSurfaceClass2 - runoffSealedSurface2;

            infiltrationFromSealedSurface3 = (
                shareOfSurfaceClass3 * imperviousnessOther * areaShareBuildings +
                shareOfRoadClass3 * imperviousnessRoad * areaShareRoads
            ) * bagrovSurfaceClass3 - runoffSealedSurface3;

            infiltrationFromSealedSurface4 = (
                shareOfSurfaceClass4 * imperviousnessOther * areaShareBuildings +
                shareOfRoadClass4 * imperviousnessRoad * areaShareRoads
            ) * bagrovSurfaceClass4 - runoffSealedSurface4;
            
            // consider unsealed road surfaces as pavement class 4
            // old: 0.11F * (1-vgs) * fsant * R4V;
            runoffPerviousRoads = 0.0F;

            // old: 0.89F * (1-vgs) * fsant * R4V;
            infiltrationFromPerviousRoads = (1 - imperviousnessRoad) *
                areaShareRoads * bagrovSurfaceClass4;

            // runoff for unsealed surfaces rowuv = 0
            infiltrationFromPerviousSurfaces = (
                100.0F - (float) resultRecord.imperviousness
            ) / 100.0F * unsealedSurfaceRunoff;

            // calculate runoff 'row' for entire block patial area (FLGES +
            // STR_FLGES) (mm/a)
            row = (
                runoffSealedSurface1 +
                runoffSealedSurface2 +
                runoffSealedSurface3 +
                runoffSealedSurface4 +
                runoffRoofs +
                runoffPerviousRoads
            );

            resultRecord.runoff = INT_ROUND(row);
            
            // calculate volume 'rowvol' from runoff (qcm/s)
            rainwaterRunoff = row * 3.171F * (
                totalAreaBuildings + totalAreaRoads
            ) / 100000.0F;
            
            // calculate infiltration rate 'ri' for entire block partial area
            // (mm/a)
            ri = (
                infiltrationFromSealedSurface1 +
                infiltrationFromSealedSurface2 +
                infiltrationFromSealedSurface3 +
                infiltrationFromSealedSurface4 +
                infiltrationFromRoofs +
                infiltrationFromPerviousRoads +
                infiltrationFromPerviousSurfaces
            );

            resultRecord.infiltrationRate = INT_ROUND(ri);
            
            // calculate volume 'rivol' from infiltration rate (qcm/s)
            totalSubsurfaceFlow = ri * 3.171F * (
                totalAreaBuildings + totalAreaRoads
            ) / 100000.0F;

            // calculate total system losses 'r' due to runoff and infiltration
            // for entire block partial area
            r = row + ri;
            resultRecord.totalSystemLosses = INT_ROUND(r);
            
            // calculate volume of system losses 'rvol' due to runoff and
            // infiltration
            totalRunoff = rainwaterRunoff + totalSubsurfaceFlow;

            // calculate total area of building development area as well as
            // roads area
            float flaeche1 = totalAreaBuildings + totalAreaRoads;

// cls_5b:
            // calculate evaporation 'verdunst' by subtracting 'r', the sum of
            // runoff and infiltration from precipitation of entire year,
            // multiplied by precipitation correction factor
            float verdunst = (
                precipitationYear *
                initValues.getPrecipitationCorrectionFactor()
            ) - r;

            // write the calculated variables into respective fields
            writer.addRecord();
            writer.setRecordField("CODE", record.code);
            writer.setRecordField("R", r);
            writer.setRecordField("ROW", row);
            writer.setRecordField("RI", ri);
            writer.setRecordField("RVOL", totalRunoff);
            writer.setRecordField("ROWVOL", rainwaterRunoff);
            writer.setRecordField("RIVOL", totalSubsurfaceFlow);
            writer.setRecordField("FLAECHE", flaeche1);
// cls_5c:
            writer.setRecordField("VERDUNSTUN", verdunst);

            index++;
        }
        else {
            counters.noUsageGiven++;

        }

        // cls_2: Hier koennten falls gewuenscht die Flaechen dokumentiert
        // werden, deren NUTZUNG=NULL (siehe auch cls_3)

        emit processSignal(
            (int)((float) k / (float) counters.recordsRead * 50.0),
            "Berechne"
        );
    }

    counters.recordsWritten = index;

    emit processSignal(50, "Schreibe Ergebnisse.");

    if (!writer.write()) {
        protocolStream << "Error: "+ writer.getError() +"\r\n";
        error = "Fehler beim Schreiben der Ergebnisse.\n" + writer.getError();
        return false;
    }

    return true;
}

// =============================================================================
// FIXME:
// =============================================================================
void Calculation::getUsage(int nutz, int typ, int f30, int f150, QString code)
{
    // mittlere pot. kapillare Aufstiegsrate d. Sommerhalbjahres
    float kr;

    // Feldlaengen von iTAS und inFK_S, L, T, U
    // extern int lenTAS, lenS, lenL, lenT, lenU;

    // declaration of yield power (ERT) and irrigation (BER) for agricultural or
    // gardening purposes
    setUsageYieldIrrigation(nutz, typ, code);

    if (resultRecord.usage != Usage::waterbody_G)
    {
        // pot. Aufstiegshoehe TAS = FLUR - mittl. Durchwurzelungstiefe TWS
        potentialCapillaryRise = resultRecord.depthToWaterTable -
                config->getTWS(resultRecord.yieldPower, resultRecord.usage);

        // Feldkapazitaet
        // cls_6b: der Fall der mit NULL belegten FELD_30 und FELD_150 Werte
        // wird hier im erten Fall behandelt - ich erwarte dann den Wert 0
        resultRecord.usableFieldCapacity = PDR::estimateWaterHoldingCapacity(
                    f30, f150, resultRecord.usage == Usage::forested_W
        );

        // mittlere pot. kapillare Aufstiegsrate kr (mm/d) des Sommerhalbjahres
        //
        // switch (bod) {
        //   case S: case U: case L: case T: case LO: case HN:
        // }
        //
        // wird eingefuegt, wenn die Bodenart in das Zahlenmaterial aufgenommen
        // wird. Vorlaeufig wird Sande angenommen.

        kr = (potentialCapillaryRise <= 0.0) ?
            7.0F :
            MEAN_POTENTIAL_CAPILLARY_RISE_RATES_SUMMER[
                Helpers::index(
                    potentialCapillaryRise,
                    POTENTIAL_RATES_OF_ASCENT,
                    n_POTENTIAL_RATES_OF_ASCENT
                ) +
                Helpers::index(
                    resultRecord.usableFieldCapacity,
                    USABLE_FIELD_CAPACITIES,
                    n_USABLE_FIELD_CAPACITIES
                ) * n_POTENTIAL_RATES_OF_ASCENT
            ];

        // mittlere pot. kapillare Aufstiegsrate kr (mm/d) des Sommerhalbjahres
        resultRecord.meanPotentialCapillaryRiseRate = (int) (
                    PDR::estimateDaysOfGrowth(
                        resultRecord.usage,
                        resultRecord.yieldPower
                    ) * kr
        );
    }

    if (initValues.getIrrigationToZero() && resultRecord.irrigation != 0) {
        //*protokollStream << "Erzwinge BER=0 fuer Code: " << code << ", Wert war:" << ptrDA.BER << " \r\n";
        counters.irrigationForcedToZero++;
        resultRecord.irrigation = 0;
    }
}

void Calculation::setUsageYieldIrrigation(int usage, int type, QString code)
{
    UsageResult result;

    result = config->getUsageResult(usage, type, code);

    if (result.tupleIndex < 0) {
        protocolStream << result.message;
        qDebug() << result.message;
       abort();
    }

    if (!result.message.isEmpty()) {
        protocolStream << result.message;
        counters.recordsProtocol++;
    }

    resultRecord.setUsageYieldIrrigation(config->getUsageTuple(result.tupleIndex));
}

// =============================================================================
// FIXME:
// =============================================================================
void Calculation::getClimaticConditions(int district, QString code)
{
    // Effectivity parameter
    float effectivityParameter;

    // Potential evaporation
    float potentialEvaporation;

    // Prepcipitation at ground level
    float precipitation;

    // ratio of precipitation to potential evaporation
    float xRatio;

    // ratio of real evaporation to potential evaporation
    float yRatio;

    // real evapotranspiration
    float realEvapotranspiration;

    // Later on two additional parameters, (now and?) here:
    // * ptrDA.P1 = p1;
    // * ptrDA.PS = ps;
    resultRecord.precipitationYear = precipitationYear;
    resultRecord.precipitationSummer = precipitationSummer;

    // Parameter for the city districts
    if (resultRecord.usage == Usage::waterbody_G)
    {
        resultRecord.longtimeMeanPotentialEvaporation = initValueOrReportedDefaultValue(
            district, code, initValues.hashEG, 775, "EG"
        );
    }
    else
    {
        resultRecord.longtimeMeanPotentialEvaporation = initValueOrReportedDefaultValue(
            district, code, initValues.hashETP, 660, "ETP"
        );

        resultRecord.potentialEvaporationSummer = initValueOrReportedDefaultValue(
            district, code, initValues.hashETPS, 530, "ETPS"
        );
    }

    // Declaration of potential evaporation and precipitation
    potentialEvaporation = (float) resultRecord.longtimeMeanPotentialEvaporation; // no more correction with 1.1
    precipitation = (float) resultRecord.precipitationYear * initValues.getPrecipitationCorrectionFactor();
      // ptrDA.KF

    // Berechnung der Abfluesse RDV und R1V bis R4V fuer versiegelte
    // Teilflaechen und unterschiedliche Bagrovwerte ND und N1 bis N4

    // ratio precipitation to potential evaporation
    xRatio = precipitation / potentialEvaporation;

    Bagrov bagrov;

    // Berechnung des Abflusses RxV fuer versiegelte Teilflaechen mittels
    // Umrechnung potentieller Verdunstungen potentialEvaporation zu realen
    // ueber Umrechnungsfaktor yRatio und subtrahiert von Niederschlag
    // precipitation

    bagrovRoof = precipitation - bagrov.nbagro(initValues.getBagrovRoof(), xRatio) * potentialEvaporation;
    bagrovSurfaceClass1 = precipitation - bagrov.nbagro(initValues.getBagrovSufaceClass1(), xRatio) * potentialEvaporation;
    bagrovSurfaceClass2 = precipitation - bagrov.nbagro(initValues.getBagrovSufaceClass2(), xRatio) * potentialEvaporation;
    bagrovSurfaceClass3 = precipitation - bagrov.nbagro(initValues.getBagrovSufaceClass3(), xRatio) * potentialEvaporation;
    bagrovSurfaceClass4 = precipitation - bagrov.nbagro(initValues.getBagrovSufaceClass4(), xRatio) * potentialEvaporation;

    // Calculate runoff RUV for unsealed partial surfaces
    if (resultRecord.usage == Usage::waterbody_G)
    {
        unsealedSurfaceRunoff = precipitation - potentialEvaporation;
    }
    else
    {
        // Determine effectiveness parameter bag for unsealed surfaces
        // Modul Raster abgespeckt
        effectivityParameter = EffectivenessUnsealed::calculate(resultRecord);

        if (resultRecord.precipitationSummer > 0 && resultRecord.potentialEvaporationSummer > 0) {
            effectivityParameter *= getSummerModificationFactor(
                (float) (resultRecord.precipitationSummer + resultRecord.irrigation + resultRecord.meanPotentialCapillaryRiseRate) / resultRecord.potentialEvaporationSummer
            );
        }

        // Calculate the x-factor of bagrov relation: x = (P + KR + BER)/ETP
        // Then get the y-factor: y = fbag(n, x)
        yRatio = bagrov.nbagro(
            effectivityParameter,
            (precipitation + resultRecord.meanPotentialCapillaryRiseRate + resultRecord.irrigation) / potentialEvaporation
        );

        // Get the real evapotransporation using estimated y-factor
        realEvapotranspiration = yRatio * potentialEvaporation;

        if (potentialCapillaryRise < 0) {
            realEvapotranspiration += (
                potentialEvaporation - yRatio * potentialEvaporation
            ) * (float) exp(resultRecord.depthToWaterTable / potentialCapillaryRise);
        }

        unsealedSurfaceRunoff = precipitation - realEvapotranspiration;
    }
}

float Calculation::initValueOrReportedDefaultValue(
        int district,
        QString code,
        QHash<int, int> &hash,
        int defaultValue,
        QString name
)
{
    // Take value from hash table (as read from xml file) if available
    if (hash.contains(district)) {
        return hash.value(district);
    }

    // Default
    float result = hash.contains(0) ? hash.value(0) : defaultValue;

    QString districtString;
    districtString.setNum(district);

    QString string;
    string.setNum(result);

    protocolStream << "\r\n" + name + " unbekannt fuer " + code +
        " von Bezirk " + districtString + "\r\n" + name +
        "=" + string + " angenommen\r\n";
    counters.recordsProtocol++;

    return result;
}

// =============================================================================
// Get factor to be applied for "summer"
// =============================================================================
float Calculation::getSummerModificationFactor(float wa)
{
    const float VALUES_WA[] =
    {
        0.45F, 0.50F, 0.55F, 0.60F, 0.65F, 0.70F, 0.75F, // 0 ..  6
        0.80F, 0.85F, 0.90F, 0.95F, 1.00F, 1.05F, 1.10F  // 7 .. 13
    };

    const float VALUES_F[] =
    {
        0.65F, 0.75F, 0.82F, 0.90F, 1.00F, 1.06F, 1.15F, // 0 ..  6
        1.22F, 1.30F, 1.38F, 1.47F, 1.55F, 1.63F, 1.70F  // 7 .. 13
    };

    return Helpers::interpolate(wa, VALUES_WA, VALUES_F, 14);
}

void Calculation::runCalculation(
        QString inputFile,
        QString configFile,
        QString outputFile,
        bool debug
)
{
    // Open the input file
    DbaseReader dbReader(inputFile);

    // Try to read the raw (text) values into the dbReader object
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
        QString error = InitValues::updateFromConfig(initValues, configFile);
        if (!error.isEmpty()) {
            qDebug() << "Error in updateFromConfig: " << error;
            abort();
        }
    }

    QFile logHandle(Helpers::defaultLogFileName(outputFile));

    Helpers::openFileOrAbort(logHandle, QFile::WriteOnly);

    QTextStream logStream(&logHandle);

    Calculation calculator(dbReader, initValues, logStream);

    bool success = calculator.calculate(outputFile, debug);

    if (!success) {
        qDebug() << "Error in calculate(): " << calculator.getError();
        abort();
    }

    logHandle.close();
}
