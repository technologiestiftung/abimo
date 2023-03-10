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

QString DbaseFile::getVersion() const
{
    return m_version;
}

QString DbaseFile::getLanguageDriver() const
{
    return m_languageDriver;
}

QDate DbaseFile::getDate() const
{
    return m_date;
}

int DbaseFile::getHeaderLength() const
{
    return m_headerLength;
}

int DbaseFile::getRecordLength() const
{
    return m_recordLength;
}

int DbaseFile::getRecordNumber() const
{
    return m_recordNumber;
}

int DbaseFile::getFieldNumber() const
{
    return m_fieldNumber;
}

QVector<QString> DbaseFile::getValues() const
{
    return m_stringValues;
}

QString DbaseFile::getError() const
{
    return m_error;
}

QString DbaseFile::getFullError() const
{
    return m_fullError;
}

int DbaseFile::expectedFileSize() const
{
    return m_headerLength + (m_recordNumber * m_recordLength) + 1;
}

int DbaseFile::calculateNumberOfFields(int headerLength)
{
    return (headerLength - m_bytesFileInfo - m_bytesTerminator) /
        m_bytesPerField;
}

int DbaseFile::calculateHeaderLength(int numberOfFields)
{
    return numberOfFields * m_bytesPerField + m_bytesFileInfo +
        m_bytesTerminator;
}
