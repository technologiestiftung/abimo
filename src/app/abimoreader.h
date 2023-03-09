#ifndef ABIMOREADER_H
#define ABIMOREADER_H

#include <QStringList>

#include "abimorecord.h"
#include "dbaseReader.h"

class AbimoReader : public DbaseReader
{
public:
    AbimoReader(const QString& file);

    static QStringList requiredFields();
    bool isAbimoFile();
    void fillRecord(int k, AbimoRecord& record, bool debug = false);
    bool checkAndRead();

private:
    // convert string to float and divide by 100
    float floatFraction(QString string);
};

#endif // ABIMOREADER_H
