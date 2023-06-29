/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef CALCULATION_H
#define CALCULATION_H

#include <array>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "abimoReader.h"
#include "abimoWriter.h"
#include "abimoInputRecord.h"
#include "abimoOutputRecord.h"
#include "usageConfiguration.h"
#include "counters.h"
#include "dbaseWriter.h"
#include "initValues.h"
#include "intermediateResults.h"
#include "soilAndVegetation.h"

class Calculation : public QObject
{
    Q_OBJECT

public:

    // Constructor
    Calculation(
        AbimoReader& dbaseReader,
        InitValues& initValues,
        QTextStream& protocolStream
    );

    // Static function to perform a "batch run"
    static void runCalculation(
        QString inputFile,
        QString configFile,
        QString outputFile,
        bool debug = false
    );

    static void runCalculationUsingData(
        QString inputFile,
        QString outputFile
    );

    static int calculateData(
        QVector<AbimoInputRecord>& inputData,
        QVector<AbimoOutputRecord>& outputData
    );

    // Main function to perform the calculation of the whole input table
    bool calculate(QString& outputFile, bool debug = false);

    // Get-functions
    Counters getCounters() const;
    QString getError() const;

    // Function to be called to stop a running calculation
    void stopProcessing();

signals:
    void processSignal(int, QString);

private:

    //
    // All private member variables of this class will be prefixed with "m_"
    //

    // Mapping between type of area and "usage tuples" consisting of three
    // elements (main usage type, yield, irrigation)
    UsageConfiguration m_usageMappings;

    // Initial values as provided in config.xml
    InitValues& m_initValues;

    // Accessor to the log file (.log)
    QTextStream& m_protocolStream;

    // Accessor to the input file (.dbf)
    AbimoReader& m_dbReader;

    // Structure holding long and short error text
    QString m_error;

    // Structure holding different counters, required for reporting
    Counters m_counters;

    // Variable to control whether to stop processing
    bool m_continueProcessing;

    // Current prefix to be used in log file
    QString m_prefix;

    //
    // Methods
    //

    static void doCalculationsFor(
        AbimoInputRecord& input,
        IntermediateResults& results,
        InitValues& initValues,
        UsageConfiguration& usageConfiguration,
        Counters& m_counters,
        QTextStream& protocolStream
    );

    static UsageTuple provideUsageInformation(
        AbimoInputRecord& input,
        UsageConfiguration& usageConfiguration,
        InitValues& initValues,
        Counters& counters,
        QTextStream& protocolStream
    );

    void logResults(AbimoInputRecord inputRecord, IntermediateResults results);
    void logVariable(QString name, float value);

    static EvaporationRelevantVariables setEvaporationVars(
        UsageTuple& usageTuple,
        AbimoInputRecord& input,
        UsageConfiguration usageConfiguration
    );

    static Precipitation getPrecipitation(
        int precipitationYear,
        int precipitationSummer,
        InitValues& initValues
    );

    static PotentialEvaporation getPotentialEvaporation(
        Usage& usage,
        InitValues& initValues,
        int district,
        QString code,
        Counters& counters,
        QTextStream& protocolStream
    );

    static float initValueOrReportedDefaultValue(
        int bez,
        QString code,
        QHash<int, int> &hash,
        int defaultValue,
        QString name,
        Counters& counters,
        QTextStream& protocolStream
    );

    static void setBagrovValues(
        Precipitation& precipitation,
        PotentialEvaporation& potentialEvaporation,
        InitValues& initValues,
        BagrovValues& bagrovValues
    );

    static void handleTotalAreaOfZero(
        AbimoInputRecord& input,
        Counters& counters
    );

    static void calculateRunoffSealed(
        AbimoInputRecord& input,
        BagrovValues& bagrovValues,
        InitValues& initValues,
        Runoff& runoff
    );

    static void calculateInfiltrationSealed(
        AbimoInputRecord& input,
        BagrovValues& bagrovValues,
        Runoff& runoff,
        Infiltration& infiltrationSealed
    );

    static float actualEvaporation(
        UsageTuple& usageTuple,
        PotentialEvaporation& potentialEvaporation,
        EvaporationRelevantVariables& evaporationVars,
        Precipitation& precipitation
    );

    static int fillResultRecord(
        AbimoInputRecord& input,
        IntermediateResults& results,
        AbimoOutputRecord& output
    );

    static void writeResultRecord(
        AbimoOutputRecord& output,
        AbimoWriter& writer
    );

    int progressNumber(int i, int n, float max);
};

#endif
