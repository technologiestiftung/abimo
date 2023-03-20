/***************************************************************************
 * For copyright information please see COPYRIGHT in the base directory
 * of this repository (https://github.com/KWB-R/abimo).
 ***************************************************************************/

#include <QByteArray>
#include <QChar>
#include <QDebug>
#include <QString>
#include <QStringList>

#include "dbaseField.h"
#include "helpers.h"

DbaseField::DbaseField():
    m_fieldLength(0),
    m_numDecimalPlaces(0)
{
}

DbaseField::DbaseField(QByteArray bytes)
{
    m_name = QString(bytes.left(10));
    m_type = QString((QChar) bytes.at(11));
    m_fieldLength = (quint8) bytes.at(16);
    m_numDecimalPlaces = (quint8) bytes.at(17);
}

DbaseField::DbaseField(QString name, QString type, int numDecimalPlaces)
{
    set(name, type, numDecimalPlaces);
}

void DbaseField::set(QString name, QString type, int numDecimalPlaces)
{
    m_name = name;
    m_type = type;
    m_fieldLength = 0;
    m_numDecimalPlaces = numDecimalPlaces;
}

QString DbaseField::getName()
{
    return m_name;
}

QString DbaseField::getType()
{
    return m_type;
}

int DbaseField::getFieldLength()
{
    return m_fieldLength;
}

void DbaseField::setFieldLength(int fieldLength)
{
    m_fieldLength = fieldLength;
}

int DbaseField::getDecimalCount()
{
    return m_numDecimalPlaces;
}

void DbaseField::formatNumericString(
    QString& s, int length, int digits, char fill, bool debug
)
{
    if (debug) {
        qDebug() << "formatNumericString(" <<
                    s << "," << length << "," << digits << "," << fill << ")";
    }

    // https://www.dbase.com/Knowledgebase/INT/db7_file_fmt.htm
    // F - Float: Number stored as a string, right justified, and padded with
    //     blanks to the width of the field.
    // N - Numeric: Number stored as a string, right justified, and padded with
    //     blanks to the width of the field.

    // s = "123.456", length = 5, decimalPlaces = 1, fill = '0'

    // left = "123"
    // leftLength = 3 (length - 1 - decimalPlaces)
    // left.rightJustified(leftLength, blank) = "123"
    // right.leftJustified(decimalPlaces, blank) = "456" and not "4"!

    if (digits <= 0) {
        s = s.rightJustified(length, fill);
        if (debug) {
            qDebug() << "return s =" << s;
        }

        return;
    }

    QStringList parts = s.split(".");

    assert(parts.length() == 2);

    int leftLength = length - 1 - digits;

    QString left = parts[0];

    left = (left.contains('-')) ?
        QString("-") +
        left.right(left.length() - 1)
        .rightJustified(leftLength - 1, fill) :
        left.rightJustified(leftLength, fill);

    if (debug) {
        qDebug() << "left =" << left;
    }

    // truncate = true!
    QString s1, s2;

    // truncate is only relevant if the length of the string is greater than
    // the target length
    if (parts[1].length() > digits) {
        qDebug() << "length > digits -> truncate = true/false will have an impact...";
    }

    s1 = left + "." + parts[1].leftJustified(digits, fill, false);
    s2 = left + "." + parts[1].leftJustified(digits, fill, true);

    if (QString::compare(s1, s2) != 0) {
        qDebug() << "s1 != s2. s1 =" << s1 << ", s2 =" << s2;
    }

    //assert(QString::compare(s1, s2));

    s = s2;

    if (debug) {
        qDebug() << "s =" << s;
    }

    // also truncate = true?
    s = s.rightJustified(length, fill);

    if (debug) {
        qDebug() << "return s =" << s;
    }
}
