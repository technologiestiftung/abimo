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
            AbimoReader &dbaseReader,
            InitValues &initValues,
            QTextStream & protocolStream
    );

    // Static function to perform a "batch run"
    static void runCalculation(
            QString inputFile,
            QString configFile,
            QString outputFile,
            bool debug = false
    );

    // Member function to perform the calculation on an instance of the
    // Calculation class
    bool calculate(QString& outputFile, bool debug = false);

    Counters getCounters() const;

    QString getError() const;

    void stopProcessing();

signals:
    void processSignal(int, QString);

private:

    // All private member variables of this class will be prefixed with "m_"
    UsageConfiguration m_usageMappings;

    InitValues& m_initValues;
    QTextStream& m_protocolStream;
    AbimoReader& m_dbReader;

    QString m_error;
    Counters m_counters;

    // Variable to control whether to stop processing
    bool m_continueProcessing;

    // Methods

    float getSummerModificationFactor(float wa);

    void logNotDefined(QString code, int type);

    UsageTuple provideUsageInformation(AbimoInputRecord& input);

    EvaporationRelevantVariables setEvaporationVars(
        UsageTuple& usageTuple,
        AbimoInputRecord& input
    );

    PotentialEvaporation getPotentialEvaporation(
        Usage& usage,
        InitValues& initValues,
        int district,
        QString code
    );

    Precipitation getPrecipitation(
        int precipitationYear,
        int precipitationSummer,
        InitValues& initValues
    );

    float realEvapotranspiration(
        PotentialEvaporation potentialEvaporation,
        Precipitation precipitation,
        float depthToWaterTable,
        EvaporationRelevantVariables& evaporationVars,
        UsageTuple& usageTuple
    );

    float initValueOrReportedDefaultValue(
        int bez,
        QString code,
        QHash<int, int> &hash,
        int defaultValue,
        QString name
    );

    int progressNumber(int i, int n, float max);

    void doCalculationsFor(
        AbimoInputRecord& input,
        IntermediateResults& results
    );

    int fillResultRecord(
        AbimoInputRecord& input,
        IntermediateResults& results,
        AbimoOutputRecord& output
    );

    void writeResultRecord(
        AbimoOutputRecord& output,
        AbimoWriter& writer
    ) const;
};

#endif
