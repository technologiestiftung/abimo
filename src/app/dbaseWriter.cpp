/***************************************************************************
 *   Copyright (C) 2009 by Meiko Rachimow, Claus Rachimow                  *
 *   This file is part of Abimo 3.2                                        *
 *   Abimo 3.2 is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <math.h>
#include <QByteArray>
#include <QChar>
#include <QDateTime>
#include <QFile>
#include <QHash>
#include <QIODevice>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>

#include "dbaseWriter.h"
#include "initvalues.h"

DbaseWriter::DbaseWriter(QString &file, InitValues &initValues):
    fileName(file),
    recNum(0)
{
    // Felder mit Namen, Typ, Nachkommastellen
    fields[0].set("CODE", "C", 0);
    fields[1].set("R", "N", initValues.getDecR());
    fields[2].set("ROW", "N", initValues.getDecROW());
    fields[3].set("RI", "N", initValues.getDecRI());
    fields[4].set("RVOL", "N", initValues.getDecRVOL());
    fields[5].set("ROWVOL", "N", initValues.getDecROWVOL());
    fields[6].set("RIVOL", "N", initValues.getDecRIVOL());
    fields[7].set("FLAECHE", "N", initValues.getDecFLAECHE());
    fields[8].set("VERDUNSTUN", "N", initValues.getDecVERDUNSTUNG());

    this->date = QDateTime::currentDateTime().date();

    // Fill the hash that assigns field numbers to field names
    for (int i = 0; i < countFields; i++) {
        hash[fields[i].getName()] = i;
    }
}

QString DbaseWriter::getError()
{
    return error;
}

bool DbaseWriter::write()
{
    QByteArray data;

    data.resize(lengthOfHeader);

    // Write the file header containing e.g. names and types of fields
    writeFileHeader(data);

    // Append the actual data
    writeFileData(data);

    QFile o_file(fileName);

    if (!o_file.open(QIODevice::WriteOnly)) {
        error = "kann Out-Datei: '" + fileName + "' nicht oeffnen\n Grund: " + o_file.error();
        return false;
    }

    o_file.write(data);
    o_file.close();

    return true;
}

int DbaseWriter::writeFileHeader(QByteArray &data)
{
    int index = 0;

    // Write start byte
    index = writeBytes(data, index, 0x03, 1);

    // Write date at bytes 1 to 3
    index = writeThreeByteDate(data, index, date);

    // Write record number at bytes 4 to 7
    index = writeFourByteInteger(data, index, recNum);

    // Write length of header at bytes 8 to 9
    index = writeTwoByteInteger(data, index, lengthOfHeader);

    // Calculate the length of one data row in bytes (1 byte separator?)
    lengthOfEachRecord = 1;

    for (int i = 0; i < countFields; i++) {
        lengthOfEachRecord += fields[i].getFieldLength();
    }

    // Write length of data row at bytes 10, 11
    index = writeTwoByteInteger(data, index, lengthOfEachRecord);

    // Write zeros at bytes 12 .. 28
    index = writeBytes(data, index, 0x00, 17);

    // wie in der input-Datei ??? byte 29 = language driver code (0x57 = ANSI)
    index = writeBytes(data, index, 0x57, 1); // byte 29
    index = writeBytes(data, index, 0x00, 2); // bytes 30, 31

    for (int i = 0; i < countFields; i++) {

        // Write name to data, fill up with zeros
        QString name = fields[i].getName();
        for (int k = 0; k < 11; k++) {
            int value = ((k < name.size())? name[k].toLatin1() : 0x00);
            index = writeBytes(data, index, value, 1);
        }

        // Write type to data, fill up with zeros
        QString type = fields[i].getType();
        index = writeBytes(data, index, type[0].toLatin1(), 1);
        index = writeBytes(data, index, 0x00, 4);

        // Write field length and decimal count to data (one byte each)
        index = writeBytes(data, index, fields[i].getFieldLength(), 1);
        index = writeBytes(data, index, fields[i].getDecimalCount(), 1);

        // Write 14 zeros
        index = writeBytes(data, index, 0x00, 14);
    }

    // Indicate the end of the header by writing a special byte
    index = writeBytes(data, index, 0x0D, 1);

    return index;
}

void DbaseWriter::writeFileData(QByteArray &data)
{
    QVector<QString> strings;

    for (int rec = 0; rec < recNum; rec++) {

        strings = record.at(rec);
        data.append(QChar(0x20));

        for (int field = 0; field < countFields; field++) {

            int fieldLength = fields[field].getFieldLength();

            if (fields[field].getDecimalCount() > 0) {
                QString str = strings.at(field);
                QStringList strlist = str.split(".");
                int frontLength = fieldLength - 1 - fields[field].getDecimalCount();
                if (strlist.at(0).contains('-')) {
                    data.append(QString("-"));
                    data.append(strlist.at(0).right(strlist.at(0).length() - 1).rightJustified(frontLength-1, QChar(0x30)));
                }
                else {
                    data.append(strlist.at(0).rightJustified(frontLength, QChar(0x30)));
                }
                data.append(".");
                data.append(strlist.at(1).leftJustified(fields[field].getDecimalCount(), QChar(0x30)));
            }
            else {
                data.append(strings.at(field).rightJustified(fieldLength, QChar(0x30)));
            }
        }
    }

    data.append(QChar(0x1A));
}

int DbaseWriter::writeBytes(QByteArray &data, int index, int value, int n_values)
{
    for (int i = index; i < index + n_values; i++) {
        data[i] = (quint8) value;
    }

    return index + n_values;
}

int DbaseWriter::writeThreeByteDate(QByteArray &data, int index, QDate date)
{
    int year = date.year();
    int year2 = year % 100;

    if (year > 2000) {
        year2 += 100;
    }

    int month = date.month();
    int day = date.day();

    data[index] = (quint8) year2;
    data[index + 1] = (quint8) month;
    data[index + 2] = (quint8) day;

    return index + 3;
}

int DbaseWriter::writeFourByteInteger(QByteArray &data, int index, int value)
{
    data[index] = (quint8) value;
    data[index + 1] = (quint8) (value >> 8);
    data[index + 2] = (quint8) (value >> 16);
    data[index + 3] = (quint8) (value >> 24);

    return index + 4;
}

int DbaseWriter::writeTwoByteInteger(QByteArray &data, int index, int value)
{
    data[index] = (quint8) value;
    data[index + 1] = (quint8)(value >> 8);

    return index + 2;
}

void DbaseWriter::addRecord()
{
    QVector<QString> v(countFields);
    record.append(v);
    recNum ++;
}

void DbaseWriter::setRecordField(int num, QString value)
{
    ((record.last()))[num] = QString(value);
    if (value.size() > fields[num].getFieldLength()) {
        fields[num].setFieldLength(value.size());
    }
}

void DbaseWriter::setRecordField(int num, float value)
{    
    int decimalCount = fields[num].getDecimalCount();

    // round
    value *= pow(10, decimalCount);
    value = round(value);
    value *= pow(10, -decimalCount);

    QString valueStr;
    valueStr.setNum(value, 'f', decimalCount);
    setRecordField(num, valueStr);
}

void DbaseWriter::setRecordField(QString name, QString value)
{
    if (hash.contains(name)) {
        setRecordField(hash[name], value);
    }
}

void DbaseWriter::setRecordField(QString name, float value)
{
    if (hash.contains(name)) {
        setRecordField(hash[name], value);
    }
}
