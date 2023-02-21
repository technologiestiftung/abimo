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

    // total written records
    int totalRecWrite;

    // total read records
    int totalRecRead;

    // Anzahl der Records fuer die BER == 0 gesetzt werden musste
    int totalBERtoZeroForced;

    // Anzahl der nicht berechneten Flaechen
    long keineFlaechenAngegeben;
    long nutzungIstNull;

    // Anzahl der Protokolleintraege
    long protcount;
};

class Calculation: public QObject
{
    Q_OBJECT

public:
    Calculation(DbaseReader & dbR, InitValues & init, QTextStream & protoStream);
    bool calc(QString fileOut, bool debug = false);
    long getProtCount();
    long getKeineFlaechenAngegeben();
    long getNutzungIstNull();
    Counters getCounters();
    QString getError();
    void stop();
    static void calculate(QString inputFile, QString configFile, QString outputFile, bool debug = false);

signals:
    void processSignal(int, QString);

private:
    Config *config;
    const static float potentialRateOfAscent[];
    const static float usableFieldCapacity[];
    const static float meanPotentialCapillaryRiseRateSummer[];
    InitValues & initValues;
    QTextStream & protokollStream;
    DbaseReader & dbReader;
    PDR ptrDA;
    QString error;

    // ******vorlaeufig aus Teilblock 0 wird fuer die Folgeblocks genommen
    float regenja, regenso;

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

    // to stop calc
    bool weiter;

    // functions
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
