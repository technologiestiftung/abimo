#include "intermediateResults.h"

IntermediateResults::IntermediateResults() :
    m_bagrovValues({0.0, 0.0, 0.0, 0.0, 0.0}),
    m_surfaceRunoffFlow_ROWVOL(0.0),
    m_infiltrationFlow_RIVOL(0.0),
    m_totalRunoffFlow_RVOL(0.0),
    m_potentialCapillaryRise_TAS(0.0),
    m_totalRunoff_R(0.0),
    m_surfaceRunoff_ROW(0.0),
    m_infiltration_RI(0.0),
    m_evaporation_VERDUNSTUN(0.0)
{
}
