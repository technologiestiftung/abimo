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
#include "abimoInputRecord.h"
#include "abimoOutputRecord.h"
#include "config.h"
#include "counters.h"
#include "dbaseWriter.h"
#include "initValues.h"
#include "intermediateResults.h"
#include "pdr.h"

class Calculation : public QObject, public IntermediateResults
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
    bool calculate(QString outputFile, bool debug = false);

    Counters getCounters() const;

    QString getError() const;

    void stopProcessing();

signals:
    void processSignal(int, QString);

private:

    // All private member variables of this class will be prefixed with "m_"
    Config m_usageMappings;

    InitValues &m_initValues;
    QTextStream &m_protocolStream;
    AbimoReader &m_dbReader;
    PDR m_resultRecord; // old: ptrDA
    QString m_error;

    // runoff for unsealed partial surfaces
    float m_unsealedSurfaceRunoff; // old: RUV

    // Regenwasserabfluss in Qubikzentimeter pro Sekunde
    float m_surfaceRunoffFlow; // old: ROWVOL

    // unterirdischer Gesamtabfluss in qcm/s
    float m_infiltrationFlow; // old: RIVOL

    // Gesamtabfluss in qcm/s
    float m_totalRunoffFlow; // old: RVOL

    // potentielle Aufstiegshoehe
    float m_potentialCapillaryRise; // old: TAS

    // Additional member variables (m_ indicates member)

    // Langjaehriger MW des Gesamtabflusses [mm/a] 004 N
    float m_totalRunoff; // old: R
    float m_surfaceRunoff;

    // Langjaehriger MW des unterird. Abflusses [mm/a] 004 N
    // old: RI
    float m_infiltration;
    float m_evaporation;

    Counters m_counters;

    // Variable to control whether to stop processing
    bool m_continueProcessing;

    // Methods

    float getSummerModificationFactor(float wa);

    void logNotDefined(QString code, int type);

    void getClimaticConditions(int bez, QString codestr, AbimoInputRecord& inputRecord);

    float realEvapotranspiration(
        float potentialEvaporation,
        float precipiation,
        AbimoInputRecord& inputRecord
    );

    float initValueOrReportedDefaultValue(
        int bez, QString code, QHash<int, int> &hash, int defaultValue,
        QString name
    );

    int progressNumber(int i, int n, float max);
    void doCalculationsFor(AbimoInputRecord& inputRecord);
    void writeResultRecord(AbimoInputRecord& inputRecord, DbaseWriter& writer);
};

#endif
