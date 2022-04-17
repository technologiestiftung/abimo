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

#ifndef DBASEREADER_H
#define DBASEREADER_H

#include <QDate>
#include <QFile>
#include <QHash>
#include <QString>

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
    float PROBAU;
    float PROVGU;
    float VGSTRASSE;
    float KAN_BEB;
    float KAN_VGU;
    float KAN_STR;
    float BELAG1;
    float BELAG2;
    float BELAG3;
    float BELAG4;
    float STR_BELAG1;
    float STR_BELAG2;
    float STR_BELAG3;
    float STR_BELAG4;
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
};

#endif
