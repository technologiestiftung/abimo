#ifndef ABIMOREADER_H
#define ABIMOREADER_H

#include "dbaseReader.h"

class AbimoReader : public DbaseReader
{
public:
    AbimoReader(const QString& file);
};

#endif // ABIMOREADER_H
