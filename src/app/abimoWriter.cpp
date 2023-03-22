#include "abimoWriter.h"
#include "dbaseWriter.h"

#include <QString>

#include "initValues.h"

AbimoWriter::AbimoWriter(QString& filePath, InitValues& initValues) :
    DbaseWriter(filePath, initValues)
{

}
