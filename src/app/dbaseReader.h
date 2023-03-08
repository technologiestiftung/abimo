/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef DBASEREADER_H
#define DBASEREADER_H

#include <array>

#include <QDate>
#include <QFile>
#include <QHash>
#include <QString>

// Fraction indicates numbers between 0 and 1 (instead of percentages)
struct abimoRecord {
    int usage; // old: NUTZUNG
    QString code; // old: CODE
    int precipitationYear; // old: REGENJA
    int precipitationSummer; // old: REGENSO
    float depthToWaterTable; // old: FLUR
    int type; // old: TYP
    int fieldCapacity_30; // old: FELD_30
    int fieldCapacity_150; // old: FELD_150
    int district; // old: BEZIRK

    //
    // degree of sealing (Versiegelungsgrad) of ...
    //

    // ... roof surfaces (Dachflaechen), share of roof area [%] 'PROBAU'
    float mainFractionBuiltSealed; // old: PROBAU_fraction, vgd

    // ... other sealed surfaces (sonst. versiegelte Flaechen)
    float mainFractionUnbuiltSealed; // old: PROVGU_fraction, vgb

    // ... roads (Strassen)
    float roadFractionSealed; // old: VGSTRASSE_fraction, vgs

    //
    // degree of canalization (Kanalisierungsgrad) for ...
    //

    // ... roof surfaces (Dachflaechen)
    float builtSealedFractionConnected; // old: KAN_BEB_fraction, kd

    // ... other sealed surfaces (sonst. versiegelte Flaechen)
    float unbuiltSealedFractionConnected; // old: KAN_VGU_fraction, kb

    // ... sealed roads (Strassen)
    float roadSealedFractionConnected; // old: KAN_STR_fraction, ks

    //
    // share of each pavement class for surfaces except roads of block area
    // Anteil der jeweiligen Belagsklasse
    //

    // old: BELAG1_fraction - BELAG4_fraction, bl1 - bl4
    // take care: index 0 not used (would represent roofs)
    // indices 1-4 represent surface classes 1-4
    std::array<float,5> unbuiltSealedFractionSurface;

    //
    // share of each road pavement class for roads of block area
    // Anteil der jeweiligen Strassenbelagsklasse
    //

    // old: STR_BELAG1_fraction - STR_BELAG4_fraction , bls1 - bls4
    std::array<float,5> roadSealedFractionSurface;

    // total area within city block, except roads
    float mainArea; // old: FLGES, fb;

    // area of roads within city block
    float roadArea; // old: STR_FLGES, fs;
};

class DbaseReader
{

public:
    DbaseReader(const QString& file);
    ~DbaseReader();
    bool read();
    QString getVersion();
    QString getLanguageDriver();
    QDate getDate();
    int getNumberOfRecords();
    int getLengthOfHeader();
    int getLengthOfEachRecord();
    int getCountFields();
    QString getRecord(int num, int field);
    QString getRecord(int num, const QString& name);
    QString getError();
    QString getFullError();
    static QStringList requiredFields();
    bool isAbimoFile();
    bool checkAndRead();
    QString* getValues();
    void fillRecord(int k, abimoRecord& record, bool debug = false);

private:

    // VARIABLES

    QFile m_file;
    QString m_version;
    QString m_languageDriver;
    QDate m_date;
    QHash<QString,int> m_hash;
    QString m_error;
    QString m_fullError;
    QString* m_values;

    // count of records in file
    int m_numberOfRecords;

    // length of the header in byte
    int m_lengthOfHeader;

    // length of a record in byte
    int m_lengthOfEachRecord;

    // count of fields
    int m_numberOfFields;

    // FUNCTIONS

    int expectedFileSize();

    // 1 byte unsigned give the version
    QString byteToVersion(quint8 byte, bool debug = true);

    // 1 byte unsigned give the Language Driver (code page)
    QString byteToLanguageDriver(quint8 byte, bool debug = true);

    // 3 byte unsigned char give the date of last edit
    QDate bytesToDate(quint8 byteYear, quint8 byteMonth, quint8 byteDay);

    // 32 bit unsigned char to int
    int bytesToInteger(quint8 byte1, quint8 byte2, quint8 byte3, quint8 byte4);

    // 16 bit unsigned char to int
    int bytesToInteger(quint8 byte1, quint8 byte2);

    // compute the count of fields
    int computeCountFields(int numBytesHeader);

    // convert string to float and divide by 100
    float floatFraction(QString string);
};

#endif
