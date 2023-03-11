#ifndef ABIMOOUTPUTRECORD_H
#define ABIMOOUTPUTRECORD_H

#include <QString>

class AbimoOutputRecord
{
public:
    AbimoOutputRecord();
    QString code_CODE; // record.code
    float totalRunoff_R; // m_totalRunoff
    float runoff_ROW; // m_surfaceRunoff
    float infiltrationRate_RI; // m_infiltration
    float totalRunoffFlow_RVOL; // m_totalRunoffFlow
    float rainwaterRunoff_ROWVOL; // m_surfaceRunoffFlow
    float totalSubsurfaceFlow_RIVOL; // m_infiltrationFlow
    float totalArea_FLAECHE; // record.totalArea()
    float evaporation_VERDUNSTUN; // m_evaporation
};

#endif // ABIMOOUTPUTRECORD_H
