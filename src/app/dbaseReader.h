/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef DBASEREADER_H
#define DBASEREADER_H

#include <QDate>
#include <QFile>
#include <QHash>
#include <QString>

#include "abimorecord.h"

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
    QString* getValues();

    // may be overridden by sub-classes
    virtual bool checkAndRead();

// members to which classes that inherit from DbaseReader have access
protected:

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
    int computeNumberOfFields(int numBytesHeader);
};

#endif
