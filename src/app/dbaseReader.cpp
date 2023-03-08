/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#include <QBuffer>
#include <QDebug>
#include <QHash>
#include <QIODevice>
#include <QStringList>
#include <QtGlobal>
#include <QVector>

#include "dbaseField.h"
#include "dbaseReader.h"
#include "helpers.h"

DbaseReader::DbaseReader(const QString& file):
    m_file(file),
    m_values(0),
    m_numberOfRecords(0),
    m_lengthOfHeader(0),
    m_lengthOfEachRecord(0),
    m_numberOfFields(0)
{}

DbaseReader::~DbaseReader()
{
    if (m_values != 0) {
        delete[] m_values;
    }
}

QString DbaseReader::getError()
{
    return m_error;
}

QString DbaseReader::getFullError()
{
    return m_fullError;
}

QString* DbaseReader::getValues()
{
    return m_values;
}

QStringList DbaseReader::requiredFields()
{
    // The conversion function used in calculation.cpp to convert string to
    // numeric is given as a comment

    return {
        "NUTZUNG",    // toInt()
        "CODE",       // none?
        "REGENJA",    // toInt()
        "REGENSO",    // toInt()
        "FLUR",       // toFloat()
        "TYP",        // toInt()
        "FELD_30",    // toInt()
        "FELD_150",   // toInt()
        "BEZIRK",     // toInt()
        "PROBAU",     // toFloat()
        "PROVGU",     // toFloat()
        "VGSTRASSE",  // toFloat()
        "KAN_BEB",    // toFloat()
        "KAN_VGU",    // toFloat()
        "KAN_STR",    // toFloat()
        "BELAG1",     // toFloat()
        "BELAG2",     // toFloat()
        "BELAG3",     // toFloat()
        "BELAG4",     // toFloat()
        "STR_BELAG1", // toFloat()
        "STR_BELAG2", // toFloat()
        "STR_BELAG3", // toFloat()
        "STR_BELAG4", // toFloat()
        "FLGES",      // toFloat()
        "STR_FLGES"   // toFloat()
      };
}

bool DbaseReader::isAbimoFile()
{
    return helpers::containsAll(m_hash, requiredFields());
}

bool DbaseReader::checkAndRead()
{
    QString fileName = m_file.fileName();
    QString text;

    if (!read()) {
        text = "Problem beim Oeffnen der Datei: '%1' aufgetreten.\nGrund: %2";
        m_fullError = text.arg(fileName, m_error);
        return false;
    }

    if (!isAbimoFile()) {
        text = "Die Datei '%1' ist kein valider 'Input File',\n";
        text += "Ueberpruefen Sie die Spaltennamen und die Vollstaendigkeit.";
        m_fullError = text.arg(fileName);
        return false;
    }

    m_fullError = "";
    return true;
}

bool DbaseReader::read()
{
    if (!m_file.open(QIODevice::ReadOnly)) {
        m_error = "Kann die Datei nicht oeffnen\n" + m_file.errorString();
        return false;
    }

    if (m_file.size() < 32) {
        m_error = "Datei unbekannten Formats.";
        return false;
    }

    QByteArray info = m_file.read(32);

    m_version = byteToVersion(info[0], false);
    m_date = bytesToDate(info[1], info[2], info[3]);
    m_numberOfRecords = bytesToInteger(info[4], info[5], info[6], info[7]);
    m_lengthOfHeader = bytesToInteger(info[8], info[9]);
    m_lengthOfEachRecord = bytesToInteger(info[10], info[11]);

    // info[12], info[13] reserved - filled with '00h'
    // info[14] - transaction flag
    // info[15] - encryption flag
    // info[16 to 19] - free record thread reserved for LAN
    // info[20 to 27] - reserved for multiuser dbase
    // info[28] MDX-flag

    m_languageDriver = byteToLanguageDriver(info[29], false);

    // info[30 - 31] reserved

    m_numberOfFields = computeCountFields(m_lengthOfHeader);

    if (m_file.size() != expectedFileSize()) {
        m_error = "Datei unbekannten Formats, falsche Groesse.\nSoll: %1\nIst: %2";
        m_error = m_error.arg(
            QString::number(expectedFileSize()),
            QString::number(m_file.size())
        );
        return false;
    }

    if (m_numberOfRecords <= 0) {
        m_error = "keine Records in der datei vorhanden.";
        return false;
    }

    if (m_lengthOfEachRecord <= 1 || m_lengthOfEachRecord > 4000) {
        m_error = "Groesse der records ist zu klein oder zu gross.";
        return false;
    }

    if (m_numberOfFields < 1) {
        m_error = "keine Felder gefunden.";
        return false;
    }

    //rest of header are field information
    QVector<DbaseField> fields;
    fields.resize(m_numberOfFields);

    for (int i = 0; i < m_numberOfFields; i++) {
        fields[i] = DbaseField(m_file.read(32));
        m_hash[fields[i].getName()] = i;
    }

    //Terminator
    m_file.read(2);

    QByteArray arr = m_file.read(m_lengthOfEachRecord * m_numberOfRecords);
    m_file.close();

    QBuffer buffer(&arr);
    buffer.open(QIODevice::ReadOnly);

    m_values = new QString[m_numberOfRecords * m_numberOfFields];

    for (int i = 0; i < m_numberOfRecords; i++) {
        for (int j = 0; j < m_numberOfFields; j++) {
            QString s = buffer.read(fields[j].getFieldLength()).trimmed();
            m_values[i * m_numberOfFields + j] = ((s.size() > 0) ? s : "0");
        }
        buffer.read(1);
    }

    buffer.close();
    return true;
}

int DbaseReader::expectedFileSize()
{
    return m_lengthOfHeader + (m_numberOfRecords * m_lengthOfEachRecord) + 1;
}

QString DbaseReader::getRecord(int num, const QString& name)
{
    if (!m_hash.contains(name)) {
        return 0;
    }

    return getRecord(num, m_hash[name]);
}

QString DbaseReader::getRecord(int num, int field)
{
    if (num >= m_numberOfRecords || field >= m_numberOfFields) {
        return 0;
    }

    return m_values[num * m_numberOfFields + field];
}

int DbaseReader::getCountFields()
{
    return m_numberOfFields;
}

QDate DbaseReader::getDate()
{
    return m_date;
}

QString DbaseReader::getLanguageDriver()
{
    return m_languageDriver;
}

QString DbaseReader::getVersion()
{
    return m_version;
}

int DbaseReader::getNumberOfRecords()
{
    return m_numberOfRecords;
}

int DbaseReader::getLengthOfHeader()
{
    return m_lengthOfHeader;
}

int DbaseReader::getLengthOfEachRecord()
{
    return m_lengthOfEachRecord;
}

int DbaseReader::bytesToInteger(quint8 byte1, quint8 byte2)
{
    return (int) (byte1 + (byte2 << 8));
}

int DbaseReader::bytesToInteger(
        quint8 byte1, quint8 byte2, quint8 byte3, quint8 byte4)
{
    return (int) (byte1 + (byte2 << 8) + (byte3 << 16) + (byte4 << 24));
}

QDate DbaseReader::bytesToDate(
        quint8 byteYear, quint8 byteMonth, quint8 byteDay)
{
    int year = (int) byteYear;

    if (year >= 100) {
        year += 1900;
    }

    return QDate(year, (int) byteMonth, (int) byteDay);
}

QString DbaseReader::byteToVersion(quint8 byte, bool debug)
{
    QHash<QString, quint8> versionIDs;

    versionIDs["FoxBase"] = 0x02;
    versionIDs["File without DBT (dBASE III w/o memo file)"] = 0x03;
    versionIDs["dBASE IV w/o memo file"] = 0x04;
    versionIDs["dBASE V w/o memo file"] = 0x05;
    versionIDs["Visual FoxPro w. DBC"] = 0x30;
    versionIDs["Visual FoxPro w. AutoIncrement field"] = 0x31;
    versionIDs[".dbv memo var size (Flagship)"] = 0x43;
    versionIDs["dBASE IV with memo"] = 0x7B;
    versionIDs["dBASE III+ with memo file"] = 0x83;
    versionIDs["dBASE IV w. memo"] = 0x8B;
    versionIDs["dBASE IV w. SQL table"] = 0x8E;
    versionIDs[".dbv and .dbt memo (Flagship)"] = 0xB3;
    versionIDs["Clipper SIX driver w. SMT memo file"] = 0xE5;
    versionIDs["FoxPro w. memo file"] = 0xF5;
    versionIDs["FoxPro ???"] = 0xFB;

    QString result = versionIDs.key(byte, "unknown version");

    if (debug) {
        qDebug() << "dbf file version: " << result;
    }

    return result;
}

// https://stackoverflow.com/questions/52590941/how-to-interpret-the-language-driver-name-in-a-dbase-dbf-file
QString DbaseReader::byteToLanguageDriver(quint8 byte, bool debug)
{
    QHash<QString, quint8> driverIDs;

    driverIDs["DOS USA code page 437"] = 0x01;
    driverIDs["DOS Multilingual code page 850"] = 0x02;
    driverIDs["Windows ANSI code page 1252"] = 0x03;
    driverIDs["Standard Macintosh"] = 0x04;
    driverIDs["ANSI"] = 0x57;
    driverIDs["EE MS-DOS code page 852"] = 0x64;
    driverIDs["Nordic MS-DOS code page 865"] = 0x65;
    driverIDs["Russian MS-DOS code page 866"] = 0x66;
    driverIDs["Icelandic MS-DOS"] = 0x67;
    driverIDs["Kamenicky (Czech) MS-DOS"] = 0x68;
    driverIDs["Mazovia (Polish) MS-DOS"] = 0x69;
    driverIDs["Greek MS-DOS (437G)"] = 0x6A;
    driverIDs["Turkish MS-DOS"] = 0x6B;
    driverIDs["Russian Macintosh"] = 0x96;
    driverIDs["Eastern European Macintosh"] = 0x97;
    driverIDs["Greek Macintosh"] = 0x98;
    driverIDs["Windows EE code page 1250"] = 0xC8;
    driverIDs["Russian Windows"] = 0xC9;
    driverIDs["Turkish Windows"] = 0xCA;
    driverIDs["Greek Windows"] = 0xCB;

    QString result = driverIDs.key(byte, "unknown language driver");

    if (debug) {
        qDebug() << "dbf language driver: " << result << " (id: " << byte << ")";
    }

    return result;
}

int DbaseReader::computeCountFields(int numBytesHeader)
{
    // each field is described by 32 bytes in the file header
    const int numBytesPerField = 32;

    // 32 bytes file information
    const int numBytesFileInfo = 32;

    // 1 byte terminator (0Dh)
    return (numBytesHeader - numBytesFileInfo - 1) / numBytesPerField;
}

void DbaseReader::fillRecord(int k, abimoRecord& record, bool debug)
{
    record.unbuiltSealedFractionSurface[1] = floatFraction(getRecord(k, "BELAG1"));
    record.unbuiltSealedFractionSurface[2] = floatFraction(getRecord(k, "BELAG2"));
    record.unbuiltSealedFractionSurface[3] = floatFraction(getRecord(k, "BELAG3"));
    record.unbuiltSealedFractionSurface[4] = floatFraction(getRecord(k, "BELAG4"));
    record.district = getRecord(k, "BEZIRK").toInt();
    record.code = getRecord(k, "CODE");
    record.fieldCapacity_150 = getRecord(k, "FELD_150").toInt();
    record.fieldCapacity_30 = getRecord(k, "FELD_30").toInt();
    record.mainArea = getRecord(k, "FLGES").toFloat();
    record.depthToWaterTable = getRecord(k, "FLUR").toFloat();
    record.builtSealedFractionConnected = floatFraction(getRecord(k, "KAN_BEB"));
    record.roadSealedFractionConnected = floatFraction(getRecord(k, "KAN_STR"));
    record.unbuiltSealedFractionConnected = floatFraction(getRecord(k, "KAN_VGU"));
    record.usage = helpers::stringToInt(
        getRecord(k, "NUTZUNG"),
        QString("k: %1, NUTZUNG = ").arg(QString::number(k)),
        debug
    );
    record.mainFractionBuiltSealed = helpers::stringToFloat(
        getRecord(k, "PROBAU"),
        QString("k: %1, PROBAU = ").arg(QString::number(k)),
        debug
    ) / 100.0F;
    record.mainFractionUnbuiltSealed = floatFraction(getRecord(k, "PROVGU"));
    record.precipitationYear = getRecord(k, "REGENJA").toInt();
    record.precipitationSummer = getRecord(k, "REGENSO").toInt();
    record.roadSealedFractionSurface[1] = floatFraction(getRecord(k, "STR_BELAG1"));
    record.roadSealedFractionSurface[2] = floatFraction(getRecord(k, "STR_BELAG2"));
    record.roadSealedFractionSurface[3] = floatFraction(getRecord(k, "STR_BELAG3"));
    record.roadSealedFractionSurface[4] = floatFraction(getRecord(k, "STR_BELAG4"));
    record.type = getRecord(k, "TYP").toInt();
    record.roadFractionSealed = floatFraction(getRecord(k, "VGSTRASSE"));
    record.roadArea = getRecord(k, "STR_FLGES").toFloat();
}

float DbaseReader::floatFraction(QString string)
{
    return (string.toFloat() / 100.0);
}
