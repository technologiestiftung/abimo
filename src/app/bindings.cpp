#include "bindings.h"

/**
 * Transform a 2D array of integers into a QVector of AbimoInputRecord objects.
 */
QVector<AbimoInputRecord>* array2AbimoInputVector(int array[][25], int numRecords) {
    QVector<AbimoInputRecord>* abimoInputVector = new QVector<AbimoInputRecord>();

    for (int i = 0; i < numRecords; i++) {
        int* row = array[i];

        // Create an AbimoInputRecord object and set its properties
        AbimoInputRecord* record = new AbimoInputRecord();

        // Set the values of each field in the AbimoInputRecord object
        record->usage = row[0];
        record->code = QString::number(row[1]);
        record->precipitationYear = row[2];
        record->precipitationSummer = row[3];
        record->depthToWaterTable = row[4];
        record->type = row[5];
        record->fieldCapacity_30 = row[6];
        record->fieldCapacity_150 = row[7];
        record->district = row[8];
        record->mainFractionBuiltSealed = row[9];
        record->mainFractionUnbuiltSealed = row[10];
        record->roadFractionSealed = row[11];
        record->builtSealedFractionConnected = row[12];
        record->unbuiltSealedFractionConnected = row[13];
        record->roadSealedFractionConnected = row[14];

        std::array<float, 5> unbuiltSealedFractionSurface;

        unbuiltSealedFractionSurface[1] = static_cast<float>(row[15]);
        unbuiltSealedFractionSurface[2] = static_cast<float>(row[16]);
        unbuiltSealedFractionSurface[3] = static_cast<float>(row[17]);
        unbuiltSealedFractionSurface[4] = static_cast<float>(row[18]);

        record->unbuiltSealedFractionSurface = unbuiltSealedFractionSurface;

        std::array<float, 5> roadSealedFractionSurface;

        roadSealedFractionSurface[1] = static_cast<float>(row[19]);
        roadSealedFractionSurface[2] = static_cast<float>(row[20]);
        roadSealedFractionSurface[3] = static_cast<float>(row[21]);
        roadSealedFractionSurface[4] = static_cast<float>(row[22]);

        record->roadSealedFractionSurface = roadSealedFractionSurface;

        record->mainArea = row[23];
        record->roadArea = row[24];

        // Add the record to the vector
        abimoInputVector->append(*record);

        // Remember to delete the record if it's no longer needed
        delete record;
    }

    return abimoInputVector;
};

/**
 * Create a QVector of AbimoOutputRecord objects
 */
QVector<AbimoOutputRecord>* createAbimoOutputRecordVector(int numRecords) {
    QVector<AbimoOutputRecord>* records = new QVector<AbimoOutputRecord>;
    records->resize(numRecords);

    for (int i = 0; i < numRecords; ++i) {
        (*records)[i] = AbimoOutputRecord();
    }

    return records;
};

/**
 * Delete a QVector of AbimoOutputRecord objects
 */
void deleteAbimoOutputRecordVector(QVector<AbimoOutputRecord>* records) {
    delete records;
};
