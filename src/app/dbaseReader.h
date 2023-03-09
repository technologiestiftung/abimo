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

    // may be overridden by sub-classes
    virtual bool checkAndRead();

    // Function to read the dbf file
    bool read();

    // Function to get one record (row)
    QString getRecord(int num, const QString& name);
    QString getRecord(int num, int field);

    // Accessor functions
    QString getVersion();
    QString getLanguageDriver();
    QDate getDate();

    int getHeaderLength();
    int getRecordLength();
    int getRecordNumber();
    int getFieldNumber();

    QString* getValues();

    QString getError();
    QString getFullError();

// members to which classes that inherit from DbaseReader have access
protected:

    // VARIABLES

    // Path to dbf file
    QFile m_file;

    // Version of dbf format, see byteToVersion() for a list
    QString m_version;

    // Language (code page) used in dbf file, see byteToLanguageDriver()
    QString m_languageDriver;

    // Date when dbf file was created
    QDate m_date;

    // length of the header in bytes
    int m_headerLength;

    // length of a record in bytes
    int m_recordLength;

    // number of records (data rows) in file
    int m_recordNumber;

    // number of fields in a record (data row)
    int m_fieldNumber;

    // Hash assigning the 0-based field indices to the field names
    QHash<QString,int> m_fieldPositionMap;

    // String values representing the data content of the dbf file
    QString* m_values;

    // Error string (short)
    QString m_error;

    // Error string (long)
    QString m_fullError;

    // FUNCTIONS

    int expectedFileSize();

    // 1 byte unsigned give the version
    QString byteToVersion(quint8 byte, bool debug = true);

    // 1 byte unsigned give the Language Driver (code page)
    QString byteToLanguageDriver(quint8 byte, bool debug = true);

    // 3 byte unsigned char give the date of last edit
    QDate bytesToDate(quint8 byteYear, quint8 byteMonth, quint8 byteDay);

    // 16 bit unsigned char to int
    int bytesToInteger(quint8 byte1, quint8 byte2);

    // 32 bit unsigned char to int
    int bytesToInteger(quint8 byte1, quint8 byte2, quint8 byte3, quint8 byte4);

    // compute the number of fields in one record (row)
    int numberOfFields(int numBytesHeader);
};

#endif
