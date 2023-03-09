#ifndef DBASEFFILE_H
#define DBASEFFILE_H

#include <QDate>
#include <QFile>
#include <QString>

class DbaseFile
{
public:
    DbaseFile(const QString& file);
    ~DbaseFile();

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

    int expectedFileSize();

    // compute the number of fields in one record (row)
    static int numberOfFields(int numBytesHeader);

protected:
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
};

#endif // DBASEFFILE_H
