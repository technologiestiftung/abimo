/***************************************************************************
 * For copyright information please see COPYRIGHT.txt in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef DBASEREADER_H
#define DBASEREADER_H

#include <QDate>
#include <QFile>
#include <QHash>
#include <QString>

// _fraction indicates numbers between 0 and 1 (instead of percentages)
struct abimoRecord {
    int NUTZUNG;
    QString CODE;
    int REGENJA;
    int REGENSO;
    float FLUR;
    int TYP;
    int FELD_30;
    int FELD_150;
    int BEZIRK;
    float PROBAU_fraction;
    float PROVGU_fraction;
    float VGSTRASSE_fraction;
    float KAN_BEB_fraction;
    float KAN_VGU_fraction;
    float KAN_STR_fraction;
    float BELAG1_fraction;
    float BELAG2_fraction;
    float BELAG3_fraction;
    float BELAG4_fraction;
    float STR_BELAG1_fraction;
    float STR_BELAG2_fraction;
    float STR_BELAG3_fraction;
    float STR_BELAG4_fraction;
    float FLGES;
    float STR_FLGES;
};

class DbaseReader
{

public:
    DbaseReader(const QString&);
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
    QString* getVals();
    void fillRecord(int k, abimoRecord& record, bool debug = false);

private:
    // VARIABLES:
    /////////////
    QFile file;
    QString version;
    QString languageDriver;
    QDate date;
    QHash<QString, int> hash;
    QString error;
    QString fullError;
    QString* vals;

    // count of records in file
    int numberOfRecords;

    // length of the header in byte
    int lengthOfHeader;

    // length of a record in byte
    int lengthOfEachRecord;

    // count of fields
    int countFields;

    // FUNCTIONS:
    /////////////

    int expectedFileSize();

    // 1 byte unsigned give the version
    QString checkVersion(quint8, bool debug = true);

    // 1 byte unsigned give the Language Driver (code page)
    QString checkLanguageDriver(quint8 i_byte, bool debug = true);

    // 3 byte unsigned char give the date of last edit
    QDate checkDate(quint8 i_year, quint8 i_month, quint8 i_day);

    // 32 bit unsigned char to int
    int check32(quint8 i1, quint8 i2, quint8 i3, quint8 i4);

    // 16 bit unsigned char to int
    int check16(quint8 i1, quint8 i2);

    // compute the count of fields
    int computeCountFields(int);

    // convert string to float and divide by 100
    float floatFraction(QString string);
};

#endif
