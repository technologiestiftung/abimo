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

    // Anzahl der Protokolleintraege
    long recordsProtocol;
};

class Calculation: public QObject
{
    Q_OBJECT

public:
    Calculation(
            DbaseReader &dbaseReader,
            InitValues &initValues,
            QTextStream & protocolStream
    );

    static void calculate(
            QString inputFile,
            QString configFile,
            QString outputFile,
            bool debug = false
    );

    bool calculate(QString fileOut, bool debug = false);
    long getNumberOfProtocolRecords();
    long getNumberOfNoAreaGiven();
    long getNumberOfNoUsageGiven();
    Counters getCounters();
    QString getError();
    void stopProcessing();

signals:
    void processSignal(int, QString);

private:
    Config *config;
    const static float POTENTIAL_RATES_OF_ASCENT[];
    const static float USABLE_FIELD_CAPACITIES[];
    const static float MEAN_POTENTIAL_CAPILLARY_RISE_RATES_SUMMER[];
    InitValues &initValues;
    QTextStream &protocolStream;
    DbaseReader &dbReader;
    PDR ptrDA;
    QString error;

    // *** vorlaeufig aus Teilblock 0 wird fuer die Folgeblocks genommen
    float precipitationYear, precipitationSummer;

    // Abfluesse nach Bagrov fuer N1 bis N4
    float RDV, R1V, R2V, R3V, R4V;

    float RUV;

    // Regenwasserabfluss in Qubikzentimeter pro Sekunde
    float ROWVOL;

    // unterirdischer Gesamtabfluss in qcm/s
    float RIVOL;

    // Gesamtabfluss in qcm/s
    float RVOL;

    // potentielle Aufstiegshoehe
    float TAS;

    // Feldlaenge von iTAS
    int lenTAS;

    // Feldlaenge von inFK_S
    int lenS;

    Counters counters;

    // Variable to control whether to stop processing
    bool continueProcessing;

    // Methods
    float getNUV(PDR &B);
    float getSummerModificationFactor(float wa);
    float getG02 (int nFK);
    void getNUTZ(int nutz, int typ, int f30, int f150, QString code);
    void setUsageYieldIrrigation(int usage, int type, QString code);
    void logNotDefined(QString code, int type);
    void getKLIMA(int bez, QString codestr);
    float initValueOrReportedDefaultValue(
        int bez, QString code, QHash<int, int> &hash, int defaultValue,
        QString name
    );
};

#endif
