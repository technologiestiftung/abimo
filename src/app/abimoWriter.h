#ifndef ABIMOWRITER_H
#define ABIMOWRITER_H

#include <QString>

#include "dbaseWriter.h"
#include "initValues.h"

class AbimoWriter : public DbaseWriter
{
public:
    AbimoWriter(QString& filePath, InitValues& initValues);
};

#endif // ABIMOWRITER_H
