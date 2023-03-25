#ifndef INTERMEDIATERESULTS_H
#define INTERMEDIATERESULTS_H

#include <array>

class IntermediateResults
{
public:
    IntermediateResults();

protected:
    // Abfluesse nach Bagrov fuer Daecher (index 0) und Oberflaechenklassen
    // 1 bis 4 (index 1 bis 4)
    std::array<float,5> m_bagrovValues;

    // Regenwasserabfluss in Qubikzentimeter pro Sekunde
    float m_surfaceRunoffFlow_ROWVOL;

    // unterirdischer Gesamtabfluss in qcm/s
    float m_infiltrationFlow_RIVOL;

    // Gesamtabfluss in qcm/s
    float m_totalRunoffFlow_RVOL;

    // potentielle Aufstiegshoehe
    float m_potentialCapillaryRise_TAS;

    // Langjaehriger MW des Gesamtabflusses [mm/a] 004 N
    float m_totalRunoff_R;

    // Langjaehriger MW des Regenwasserabflusses [mm/a] 003 N
    float m_surfaceRunoff_ROW;

    // Langjaehriger MW des unterird. Abflusses [mm/a] 004 N
    float m_infiltration_RI;

    float m_evaporation_VERDUNSTUN;
};

#endif // INTERMEDIATERESULTS_H
