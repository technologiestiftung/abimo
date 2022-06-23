/***************************************************************************
 * For copyright information please see COPYRIGHT.txt in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef DBASEWRITER_H
#define DBASEWRITER_H

#include <QByteArray>
#include <QDate>
#include <QHash>
#include <QString>
#include <QVector>

#include "dbaseField.h"
#include "initvalues.h"

const int countFields = 9;
const int lengthOfHeader = countFields * 32 + 32 + 1;

class DbaseWriter
{

public:
    DbaseWriter(QString &file, InitValues &initValues);
    bool write();
    void addRecord();
    void setRecordField(int num, QString value);
    void setRecordField(QString name, QString value);
    void setRecordField(int num, float value);
    void setRecordField(QString name, float value);
    QString getError();

private:
    QString fileName;
    QVector< QVector<QString> > record;
    QDate date;
    QHash<QString, int> hash;
    QString error;
    int lengthOfEachRecord;
    int recNum;
    DbaseField fields[countFields];
    int writeFileHeader(QByteArray &data);
    void writeFileData(QByteArray &data);
    int writeBytes(QByteArray &data, int index, int value, int n_values);
    int writeThreeByteDate(QByteArray &data, int index, QDate date);
    int writeFourByteInteger(QByteArray &data, int index, int value);
    int writeTwoByteInteger(QByteArray &data, int index, int value);
};

#endif
