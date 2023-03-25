#ifndef INTERMEDIATERESULTS_H
#define INTERMEDIATERESULTS_H

#include <array>

struct IntermediateResults
{
    // Abfluesse nach Bagrov fuer Daecher (index 0) und Oberflaechenklassen
    // 1 bis 4 (index 1 bis 4)
    std::array<float,5> bagrovValues = {0.0, 0.0, 0.0, 0.0, 0.0};

    // Regenwasserabfluss in Qubikzentimeter pro Sekunde
    float surfaceRunoffFlow_ROWVOL = 0.0f;

    // unterirdischer Gesamtabfluss in qcm/s
    float infiltrationFlow_RIVOL = 0.0f;

    // Gesamtabfluss in qcm/s
    float totalRunoffFlow_RVOL = 0.0f;

    // Langjaehriger MW des Gesamtabflusses [mm/a] 004 N
    float totalRunoff_R = 0.0f;

    // Langjaehriger MW des Regenwasserabflusses [mm/a] 003 N
    float surfaceRunoff_ROW = 0.0f;

    // Langjaehriger MW des unterird. Abflusses [mm/a] 004 N
    float infiltration_RI = 0.0f;

    float evaporation_VERDUNSTUN = 0.0f;
};

#endif // INTERMEDIATERESULTS_H
