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

    // runoff for unsealed partial surfaces
    float m_unsealedSurfaceRunoff; // old: RUV

    // Regenwasserabfluss in Qubikzentimeter pro Sekunde
    float m_surfaceRunoffFlow; // old: ROWVOL

    // unterirdischer Gesamtabfluss in qcm/s
    float m_infiltrationFlow; // old: RIVOL

    // Gesamtabfluss in qcm/s
    float m_totalRunoffFlow; // old: RVOL

    // potentielle Aufstiegshoehe
    float m_potentialCapillaryRise; // old: TAS

    // Additional member variables (m_ indicates member)

    // Langjaehriger MW des Gesamtabflusses [mm/a] 004 N
    float m_totalRunoff; // old: R

    // Langjaehriger MW des Regenwasserabflusses [mm/a] 003 N
    float m_surfaceRunoff;

    // Langjaehriger MW des unterird. Abflusses [mm/a] 004 N
    // old: RI
    float m_infiltration;
    float m_evaporation;
};

#endif // INTERMEDIATERESULTS_H
