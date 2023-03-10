#include "dbaseFile.h"

DbaseFile::DbaseFile(const QString& filePath) :
    m_file(filePath),
    m_headerLength(0),
    m_recordLength(0),
    m_recordNumber(0),
    m_fieldNumber(0),
    m_stringValues()
{
}

QString DbaseFile::getVersion()
{
    return m_version;
}

QString DbaseFile::getLanguageDriver()
{
    return m_languageDriver;
}

QDate DbaseFile::getDate()
{
    return m_date;
}

int DbaseFile::getHeaderLength()
{
    return m_headerLength;
}

int DbaseFile::getRecordLength()
{
    return m_recordLength;
}

int DbaseFile::getRecordNumber()
{
    return m_recordNumber;
}

int DbaseFile::getFieldNumber()
{
    return m_fieldNumber;
}

QVector<QString> DbaseFile::getValues()
{
    return m_stringValues;
}

QString DbaseFile::getError()
{
    return m_error;
}

QString DbaseFile::getFullError()
{
    return m_fullError;
}

int DbaseFile::expectedFileSize()
{
    return m_headerLength + (m_recordNumber * m_recordLength) + 1;
}

int DbaseFile::numberOfFields(int numBytesHeader)
{
    // each field is described by 32 bytes in the file header
    const int numBytesPerField = 32;

    // 32 bytes file information
    const int numBytesFileInfo = 32;

    // 1 byte terminator (0Dh)
    return (numBytesHeader - numBytesFileInfo - 1) / numBytesPerField;
}
