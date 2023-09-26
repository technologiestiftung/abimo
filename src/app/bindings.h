#ifndef ABIMO_CONVERTER_H
#define ABIMO_CONVERTER_H

#include <QVector>
#include "abimoInputRecord.h"
#include "abimoOutputRecord.h"

extern "C" Q_DECL_EXPORT QVector<AbimoInputRecord>* array2AbimoInputVector(
    int array[][25],
    int numRecords
);

extern "C" Q_DECL_EXPORT QVector<AbimoOutputRecord>* createAbimoOutputRecordVector(
    int numRecords
);

extern "C" Q_DECL_EXPORT void deleteAbimoInputRecordVector(
    QVector<AbimoInputRecord>* records
);

extern "C" Q_DECL_EXPORT void deleteAbimoOutputRecordVector(
    QVector<AbimoOutputRecord>* records
);

#endif // ABIMO_CONVERTER_H
