#ifndef ABIMO_CONVERTER_H
#define ABIMO_CONVERTER_H

#include <QVector>
#include "abimoInputRecord.h"

extern "C" Q_DECL_EXPORT  QVector<AbimoInputRecord>* array2AbimoInputVector(
    int array[][25],
    int numRows
);

#endif // ABIMO_CONVERTER_H