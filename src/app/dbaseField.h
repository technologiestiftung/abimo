/***************************************************************************
 * For copyright information please see COPYRIGHT.txt in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#ifndef DBASEFIELD_H
#define DBASEFIELD_H

#include <QByteArray>
#include <QString>

class DbaseField
{

public:
    DbaseField();
    DbaseField(QByteArray);
    DbaseField(QString name, QString type, int decimalCount);
    ~DbaseField();
    QString getName();
    QString getType();
    int getFieldLength();
    int getDecimalCount();
    void setFieldLength(int);
    void set(QString name, QString type, int decimalCount);

private:
    QString name;
    QString type;
    int fieldLength;
    int decimalCount;
};

#endif
