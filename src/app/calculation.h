/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QString>
#include <QTextStream>

#include "dbaseReader.h"
#include "dbaseWriter.h"
#include "initvalues.h"
#include "config.h"

struct Counters {

    // Total records written
    int recordsWritten;

    // Total records read
    int recordsRead;

    // Number of records for which irrigation (BER) was set to zero
    int irrigationForcedToZero;

    // Number of cases in which no calculation was performed
    long noAreaGiven;
    long noUsageGiven;

    // Number of records written to the (error) protocol
    long recordsProtocol;
};

class Calculation: public QObject
{
    Q_OBJECT

public:

    // Constructor
    Calculation(
            DbaseReader &dbaseReader,
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

    // Accessor functions to specific counters (never used!)
    long getNumberOfProtocolRecords();
    long getNumberOfNoAreaGiven();
    long getNumberOfNoUsageGiven();

    Counters getCounters();

    QString getError();

    void stopProcessing();

signals:
    void processSignal(int, QString);

private:

    // All private member variables of this class will be prefixed with "m_"
    Config m_config;

    InitValues &m_initValues;
    QTextStream &m_protocolStream;
    DbaseReader &m_dbReader;
    PDR m_resultRecord; // old: ptrDA
    QString m_error;

    // *** vorlaeufig aus Teilblock 0 wird fuer die Folgeblocks genommen
    float m_precipitationYear; // old: regenja
    float m_precipitationSummer; // old: regenso

    // Abfluesse nach Bagrov fuer N1 bis N4
    float m_bagrovValueRoof; // old: RDV
    float m_bagrovValueSurface1; // old: R1V
    float m_bagrovValueSurface2; // old: R2V
    float m_bagrovValueSurface3; // old: R3V
    float m_bagrovValueSurface4; // old: R4V

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
    float m_infiltration;
    float m_totalArea;
    float m_evaporation;

    Counters m_counters;

    // Variable to control whether to stop processing
    bool m_continueProcessing;

    // Methods

    float getSummerModificationFactor(float wa);

    void logNotDefined(QString code, int type);

    void getClimaticConditions(int bez, QString codestr);

    float initValueOrReportedDefaultValue(
        int bez, QString code, QHash<int, int> &hash, int defaultValue,
        QString name
    );

    void calculateResultRecord(abimoRecord &record);
    void writeResultRecord(abimoRecord &record, DbaseWriter &writer);
};

#endif
