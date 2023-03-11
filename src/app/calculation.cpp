// ***************************************************************************
// * For copyright information please see COPYRIGHT in the base directory
// * of this repository (https://github.com/KWB-R/abimo).
// ***************************************************************************

#include <vector>

#include <math.h>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include "abimoReader.h"
#include "abimoInputRecord.h"
#include "abimoOutputRecord.h"
#include "bagrov.h"
#include "calculation.h"
#include "config.h"
#include "constants.h"
#include "counters.h"
#include "dbaseWriter.h"
#include "effectivenessUnsealed.h"
#include "helpers.h"
#include "initValues.h"
#include "intermediateResults.h"
#include "pdr.h"

Calculation::Calculation(
    AbimoReader& dbaseReader,
    InitValues& initValues,
    QTextStream& protocolStream
) :
    IntermediateResults(),
    m_initValues(initValues),
    m_protocolStream(protocolStream),
    m_dbReader(dbaseReader),
    m_counters(),
    m_continueProcessing(true)
{
}

void Calculation::stopProcessing()
{
    m_continueProcessing = false;
}

Counters Calculation::getCounters() const
{
    return m_counters;
}

QString Calculation::getError() const
{
    return m_error;
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
    AbimoInputRecord inputRecord;
    AbimoOutputRecord outputRecord;

    // Number of processed records
    int numProcessed = 0;

    // Initialise counters
    m_counters.initialise();

    // Provide a dbf writer object
    DbaseWriter writer(outputFile, m_initValues);

    // Get the number of rows in the input data
    int recordCount = m_dbReader.getNumberOfRecords();

    // loop over all block partial areas (= records/rows of input data)
    for (int i = 0; i < recordCount; i++) {

        // Break out of the loop if the user pressed "Cancel"
        if (!m_continueProcessing) {
            break;
        }

        // Fill record with data from the current row i
        m_dbReader.fillRecord(i, inputRecord, debug);

        // usage = integer representing the type of area usage for the current
        // block partial area
        if (inputRecord.usage == 0) {

            // Hier koennten falls gewuenscht die Flaechen dokumentiert
            // werden, deren NUTZUNG = NULL
            m_counters.incrementNoUsageGiven();
        }
        else {

            // Calculate and set result record fields to calculated values
            calculateResultRecord(inputRecord);

            fillResultRecord(inputRecord, outputRecord);

            // Set the corresponding row in the result data structure
            writeResultRecord(outputRecord, writer);

            // Increment the number of processed data rows
            numProcessed++;
        }

        // Send a signal to the progress bar dialog
        emit processSignal(progressNumber(i, recordCount, 50.0), "Berechne");
    }

    // Set counters
    m_counters.setRecordsRead(recordCount);
    m_counters.setRecordsWritten(numProcessed);

    if (!m_continueProcessing) {
        m_protocolStream << "Berechnungen abgebrochen.\r\n";
        return true;
    }

    emit processSignal(50, "Schreibe Ergebnisse.");

    if (!writer.write()) {
        m_protocolStream << "Error: " + writer.getError() +"\r\n";
        m_error = "Fehler beim Schreiben der Ergebnisse.\n" + writer.getError();
        return false;
    }

    return true;
}

int Calculation::progressNumber(int i, int n, float max)
{
    return (int) (static_cast<float>(i) / static_cast<float>(n) * max);
}

void Calculation::calculateResultRecord(AbimoInputRecord& inputRecord)
{
    // Abflussvariablen der versiegelten Flaechen
    // runoff variables of sealed surfaces
    // Take care: for consistency use indices 1 to 4 only, do not use 0 (roofs)!
    // old: row1 - row4
    std::vector<float> runoffSealedSurfaces = {0, 0, 0, 0, 0};

    // Infiltrationsvariablen der versiegelten Flaechen
    // infiltration variables of sealed surfaces
    // Take care: for consistency use indices 1 to 4 only, do not use 0 (roofs)!
    // old: ri1 - ri4
    std::vector<float> infiltrationSealedSurfaces = {0, 0, 0, 0, 0};

    // declaration of yield power (ERT) and irrigation (BER) for agricultural or
    // gardening purposes
    UsageResult usageResult = m_usageMappings.getUsageResult(
        inputRecord.usage, inputRecord.type, inputRecord.code
    );

    if (usageResult.tupleIndex < 0) {
        m_protocolStream << usageResult.message;
        qDebug() << usageResult.message;
        abort();
    }

    if (!usageResult.message.isEmpty()) {
        m_protocolStream << usageResult.message;
        m_counters.incrementRecordsProtocol();
    }

    m_resultRecord.setUsageYieldIrrigation(
        m_usageMappings.getUsageTuple(usageResult.tupleIndex)
    );

    if (m_resultRecord.usage != Usage::waterbody_G)
    {
        // Feldkapazitaet
        m_resultRecord.usableFieldCapacity = PDR::estimateWaterHoldingCapacity(
            inputRecord.fieldCapacity_30,
            inputRecord.fieldCapacity_150,
            m_resultRecord.usage == Usage::forested_W
        );

        // mittl. Durchwurzelungstiefe TWS
        float rootingDepth = m_usageMappings.getRootingDepth(
            m_resultRecord.usage,
            m_resultRecord.yieldPower
        );

        // pot. Aufstiegshoehe TAS = FLUR - mittl. Durchwurzelungstiefe TWS
        m_potentialCapillaryRise_TAS = inputRecord.depthToWaterTable - rootingDepth;

        // mittlere pot. kapillare Aufstiegsrate kr (mm/d) des Sommerhalbjahres
        m_resultRecord.meanPotentialCapillaryRiseRate =
            PDR::getMeanPotentialCapillaryRiseRate(
                m_potentialCapillaryRise_TAS,
                m_resultRecord.usableFieldCapacity,
                m_resultRecord.usage,
                m_resultRecord.yieldPower
            );
    }

    if (m_initValues.getIrrigationToZero() && m_resultRecord.irrigation != 0) {
        //*protokollStream << "Erzwinge BER=0 fuer Code: " << code << ", Wert war:" << ptrDA.BER << " \r\n";
        m_counters.incrementIrrigationForcedToZero();
        m_resultRecord.irrigation = 0;
    }

    Precipitation precipitation = getPrecipitation(
        static_cast<float>(inputRecord.precipitationYear), m_initValues
    );

    PotentialEvaporation potentialEvaporation = getPotentialEvaporation(
        m_resultRecord.usage, m_initValues, inputRecord.district, inputRecord.code
    );

    // Bagrov-calculation for sealed surfaces
    getClimaticConditions(precipitation, potentialEvaporation, inputRecord);

    // percentage of total sealed area
    // share of roof area [%] 'PROBAU' +
    // share of other (unbuilt) sealed areas (e.g. Hofflaechen)
    m_resultRecord.mainPercentageSealed = helpers::roundToInteger(
        inputRecord.mainFractionBuiltSealed * 100 +
        inputRecord.mainFractionUnbuiltSealed * 100
    );

    // if sum of total building development area and road area is
    // inconsiderably small it is assumed, that the area is unknown and
    // 100 % building development area will be given by default
    if (inputRecord.totalArea_FLAECHE() < 0.0001) {
        // *protokollStream << "\r\nDie Flaeche des Elements " +
        // record.CODE + " ist 0 \r\nund wird automatisch auf 100 gesetzt\r\n";
        m_counters.incrementRecordsProtocol();
        m_counters.incrementNoAreaGiven();
        inputRecord.mainArea = 100.0F;
    }

    // share of building development area / road area to total area

    // Verhaeltnis Bebauungsflaeche zu Gesamtflaeche
    // Fraction of total area that is not allocated to roads
    // old: fbant (ant = Anteil)
    float areaFractionMain = inputRecord.fractionOfTotalArea(inputRecord.mainArea);

    // Verhaeltnis Strassenflaeche zu Gesamtflaeche
    // Fraction of total area that is allocated to roads
    // old: fsant (ant = Anteil)
    float areaFractionRoad = inputRecord.fractionOfTotalArea(inputRecord.roadArea);

    // Runoff for sealed surfaces

    // Legende der Abflussberechnung der 4 Belagsklassen bzw. Dachklasse:
    // - rowd / rowx: Abfluss Dachflaeche / Abfluss Belagsflaeche x
    // - infdach / infbelx: Infiltrationsparameter Dachfl. / Belagsfl. x
    // - belx: Anteil Belagsklasse x
    // - blsx: Anteil Strassenbelagsklasse x
    // - vgd / vgb: Anteil versiegelte Dachfl. / sonstige versiegelte Flaeche zu Gesamtblockteilflaeche
    // - kd / kb / ks: Grad der Kanalisierung Dach / sonst. vers. Fl. / Strassenflaechen
    // - fbant / fsant: ?
    // - RDV / RxV: Gesamtabfluss versiegelte Flaeche

    // runoff from roof surfaces (Abfluss der Dachflaechen)
    // old: rowd
    float runoffRoofs =
        (1.0F - m_initValues.getInfiltrationFactor(0)) * // 0 = roof!
        inputRecord.mainFractionBuiltSealed *
        inputRecord.builtSealedFractionConnected *
        areaFractionMain *
        m_bagrovValues[0]; // 0 = roof!

    for (int i = 1; i < static_cast<int>(runoffSealedSurfaces.size()); i++) {

        runoffSealedSurfaces[i] =
            (1.0F - m_initValues.getInfiltrationFactor(i)) *
            (
                inputRecord.unbuiltSealedFractionSurface.at(i) *
                inputRecord.unbuiltSealedFractionConnected *
                inputRecord.mainFractionUnbuiltSealed *
                areaFractionMain +
                inputRecord.roadSealedFractionSurface.at(i) *
                inputRecord.roadSealedFractionConnected *
                inputRecord.roadFractionSealed *
                areaFractionRoad
            ) * m_bagrovValues[i];
    }

    // infiltration of/for/from? roof surfaces (Infiltration der Dachflaechen)
    // old: rid
    float infiltrationRoofs =
        (1 - inputRecord.builtSealedFractionConnected) *
        inputRecord.mainFractionBuiltSealed *
        areaFractionMain *
        m_bagrovValues[0]; // 0 = roof

    for (int i = 1; i < static_cast<int>(infiltrationSealedSurfaces.size()); i++) {

        infiltrationSealedSurfaces[i] = (
            inputRecord.unbuiltSealedFractionSurface.at(i) *
            inputRecord.mainFractionUnbuiltSealed *
            areaFractionMain +
            inputRecord.roadSealedFractionSurface.at(i) *
            inputRecord.roadFractionSealed *
            areaFractionRoad
        ) * m_bagrovValues[i] - runoffSealedSurfaces[i];
    }

    // Abfluss von unversiegelten Strassenflaechen
    // runoff from unsealed road surfaces
    // old: rowuvs
    // consider unsealed road surfaces as pavement class 4 (???)
    // old: 0.11F * (1-vgs) * fsant * R4V;
    float runoffPerviousRoads = 0.0F;

    // Infiltration von unversiegelten Strassenflaechen
    // infiltration for/from unsealed road surfaces
    // old: riuvs
    // old: 0.89F * (1-vgs) * fsant * R4V;
    float infiltrationPerviousRoads =
            (1 - inputRecord.roadFractionSealed) *
            areaFractionRoad *
            m_bagrovValues[4];

    // Infiltration unversiegelter Flaechen
    // infiltration of unsealed areas
    // old: riuv
    // runoff for unsealed surfaces rowuv = 0 (???)
    float infiltrationPerviousSurfaces = (
        100.0F - static_cast<float>(m_resultRecord.mainPercentageSealed)
    ) / 100.0F * m_unsealedSurfaceRunoff_RUV;

    // calculate runoff 'ROW' for entire block patial area (FLGES +
    // STR_FLGES) (mm/a)
    m_surfaceRunoff_ROW = (
        helpers::vectorSum(runoffSealedSurfaces) +
        runoffRoofs +
        runoffPerviousRoads
    );

    // calculate volume 'rowvol' from runoff (qcm/s)
    m_surfaceRunoffFlow_ROWVOL = inputRecord.yearlyHeightToVolumeFlow(m_surfaceRunoff_ROW);

    // calculate infiltration rate 'ri' for entire block partial area
    // (mm/a)
    m_infiltration_RI = (
        helpers::vectorSum(infiltrationSealedSurfaces) +
        infiltrationRoofs +
        infiltrationPerviousRoads +
        infiltrationPerviousSurfaces
    );

    // calculate volume 'rivol' from infiltration rate (qcm/s)
    m_infiltrationFlow_RIVOL = inputRecord.yearlyHeightToVolumeFlow(m_infiltration_RI);

    // calculate total system losses 'r' due to runoff and infiltration
    // for entire block partial area
    m_totalRunoff_R = m_surfaceRunoff_ROW + m_infiltration_RI;

    // set totalRunoff in the result record
    //resultRecord.totalRunoff = INT_ROUND(totalRunoff);

    // calculate volume of system losses 'rvol' due to runoff and
    // infiltration
    m_totalRunoffFlow_RVOL = m_surfaceRunoffFlow_ROWVOL + m_infiltrationFlow_RIVOL;

    // calculate evaporation 'VERDUNST' by subtracting 'R', the sum of
    // runoff and infiltration from precipitation of entire year,
    // multiplied by precipitation correction factor
    m_evaporation_VERDUNSTUN = (
        static_cast<float>(inputRecord.precipitationYear) *
        m_initValues.getPrecipitationCorrectionFactor()
    ) - m_totalRunoff_R;

}

// =============================================================================
// This function uses...
// This function modifies...
// =============================================================================
void Calculation::getClimaticConditions(
    Precipitation precipitation,
    PotentialEvaporation potentialEvaporation,
    AbimoInputRecord& record
)
{
    // Berechnung der Abfluesse RDV und R1V bis R4V fuer versiegelte
    // Teilflaechen und unterschiedliche Bagrovwerte ND und N1 bis N4

    // ratio of precipitation to potential evaporation
    float xRatio =
        precipitation.perYearCorrectedFloat /
        potentialEvaporation.perYearFloat;

    // Berechnung des Abflusses RxV fuer versiegelte Teilflaechen mittels
    // Umrechnung potentieller Verdunstungen potentialEvaporation zu realen
    // ueber Umrechnungsfaktor yRatio und subtrahiert von Niederschlag
    // precipitation
    // index 0 = roof, indices 1 - 4 = surface classes 1 - 4

    for (int i = 0; i < static_cast<int>(m_bagrovValues.size()); i++) {

        m_bagrovValues[i] = precipitation.perYearCorrectedFloat -
            Bagrov::nbagro(m_initValues.getBagrovValue(i), xRatio) *
            potentialEvaporation.perYearFloat;
    }

    // Calculate runoff RUV for unsealed surfaces
    float actualEvaporation = (m_resultRecord.usage == Usage::waterbody_G) ?
        potentialEvaporation.perYearFloat :
        realEvapotranspiration(potentialEvaporation, precipitation, record);

    m_unsealedSurfaceRunoff_RUV =
        precipitation.perYearCorrectedFloat - actualEvaporation;
}

PotentialEvaporation Calculation::getPotentialEvaporation(
    Usage& usage, InitValues& initValues, int district, QString code
)
{
    PotentialEvaporation result;

    // Parameter for the city districts
    if (usage == Usage::waterbody_G) {

        result.perYearInteger = initValueOrReportedDefaultValue(
            district, code, initValues.hashEG, 775, "EG"
        );

        // What about potentialEvaporationSummer?
        result.inSummerInteger = -1;
    }
    else {
        result.perYearInteger = initValueOrReportedDefaultValue(
            district, code, initValues.hashETP, 660, "ETP"
        );
        result.inSummerInteger = initValueOrReportedDefaultValue(
            district, code, initValues.hashETPS, 530, "ETPS"
        );
    }

    // no more correction with 1.1
    result.perYearFloat = static_cast<float>(result.perYearInteger);

    return result;
}

Precipitation Calculation::getPrecipitation(
    int precipitationYear, InitValues& initValues
)
{
    Precipitation result;

    // precipitation (at ground level)
    result.perYearCorrectedFloat = static_cast<float>(
        precipitationYear * initValues.getPrecipitationCorrectionFactor()
    );

    return result;
}

float Calculation::realEvapotranspiration(
    PotentialEvaporation potentialEvaporation,
    Precipitation precipitation,
    AbimoInputRecord& record
)
{
    assert(potentialEvaporation.perYearFloat > 0.0);

    // Determine effectivity/effectiveness ??? parameter (old???: bag) for
    // unsealed surfaces
    // Modul Raster abgespeckt (???)
    float effectivityParameter = EffectivenessUnsealed::getEffectivityParameter(
        m_resultRecord.usableFieldCapacity,
        m_resultRecord.usage == Usage::forested_W,
        m_resultRecord.yieldPower,
        m_resultRecord.irrigation,
        static_cast<float>(precipitation.inSummerInteger),
        potentialEvaporation.inSummerInteger,
        m_resultRecord.meanPotentialCapillaryRiseRate
    );

    // Calculate the x-factor of bagrov relation: x = (P + KR + BER)/ETP
    // Then get the y-factor: y = fbag(n, x)
    // ratio of real evaporation to potential evaporation
    float yRatio = Bagrov::nbagro(
        effectivityParameter,
        (
            precipitation.perYearCorrectedFloat +
            m_resultRecord.meanPotentialCapillaryRiseRate +
            m_resultRecord.irrigation
        ) / potentialEvaporation.perYearFloat
    );

    // Get the real evapotransporation using estimated y-factor
    float result = yRatio * potentialEvaporation.perYearFloat;

    if (m_potentialCapillaryRise_TAS < 0) {
        result +=
            (potentialEvaporation.perYearFloat - result) *
            static_cast<float>(
                exp(record.depthToWaterTable / m_potentialCapillaryRise_TAS)
            );
    }

    return result;
}

float Calculation::initValueOrReportedDefaultValue(
        int district,
        QString code,
        QHash<int,int> &hash,
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

    QString resultString;
    resultString.setNum(result);

    m_protocolStream << QString(
        "\r\n%1 unbekannt fuer %2 von Bezirk %3\r\n%4=%5 angenommen\r\n"
    ).arg(name, code, districtString, name, resultString);

    m_counters.incrementRecordsProtocol();

    return result;
}

int Calculation::fillResultRecord(
    AbimoInputRecord& inputRecord,
    AbimoOutputRecord& outputRecord
)
{
    outputRecord.code_CODE = inputRecord.code;
    outputRecord.totalRunoff_R = m_totalRunoff_R;
    outputRecord.runoff_ROW = m_surfaceRunoff_ROW;
    outputRecord.infiltrationRate_RI = m_infiltration_RI;
    outputRecord.totalRunoffFlow_RVOL = m_totalRunoffFlow_RVOL;
    outputRecord.rainwaterRunoff_ROWVOL = m_surfaceRunoffFlow_ROWVOL;
    outputRecord.totalSubsurfaceFlow_RIVOL = m_infiltrationFlow_RIVOL;
    outputRecord.totalArea_FLAECHE = inputRecord.totalArea_FLAECHE();
    outputRecord.evaporation_VERDUNSTUN = m_evaporation_VERDUNSTUN;

    return 0;
}

void Calculation::writeResultRecord(
    AbimoOutputRecord& record,
    DbaseWriter& writer
)
{
    writer.addRecord();
    writer.setRecordField("CODE", record.code_CODE);
    writer.setRecordField("R", record.totalRunoff_R);
    writer.setRecordField("ROW", record.runoff_ROW);
    writer.setRecordField("RI", record.infiltrationRate_RI);
    writer.setRecordField("RVOL", record.totalRunoffFlow_RVOL);
    writer.setRecordField("ROWVOL", record.rainwaterRunoff_ROWVOL);
    writer.setRecordField("RIVOL", record.totalSubsurfaceFlow_RIVOL);
    writer.setRecordField("FLAECHE", record.totalArea_FLAECHE);
    writer.setRecordField("VERDUNSTUN", record.evaporation_VERDUNSTUN);
}

void Calculation::runCalculation(
        QString inputFile,
        QString configFile,
        QString outputFile,
        bool debug
)
{
    // Open the input file
    AbimoReader dbReader(inputFile);

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

    QFile logHandle(helpers::defaultLogFileName(outputFile));

    helpers::openFileOrAbort(logHandle, QFile::WriteOnly);

    QTextStream logStream(&logHandle);

    Calculation calculator(dbReader, initValues, logStream);

    bool success = calculator.calculate(outputFile, debug);

    if (!success) {
        qDebug() << "Error in calculate(): " << calculator.getError();
        abort();
    }

    logHandle.close();
}
