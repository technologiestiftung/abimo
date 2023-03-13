#include "abimoWriter.h"
#include "dbaseWriter.h"

#include <QString>

#include "dbaseField.h"
#include "initValues.h"

AbimoWriter::AbimoWriter(QString& filePath, InitValues& initValues) :
    DbaseWriter(filePath)
{
    // Felder mit Namen, Typ, Nachkommastellen
    defineField("CODE", "C", 0);
    defineField("R", "N", initValues.getDigitsTotalRunoff_R());
    defineField("ROW", "N", initValues.getDigitsSurfaceRunoff_ROW());
    defineField("RI", "N", initValues.getDigitsInfiltration_RI());
    defineField("RVOL", "N", initValues.getDigitsTotalRunoffFlow_RVOL());
    defineField("ROWVOL", "N", initValues.getDigitsSurfaceRunoffFlow_ROWVOL());
    defineField("RIVOL", "N", initValues.getDigitsInfiltrationFlow_RIVOL());
    defineField("FLAECHE", "N", initValues.getDigitsTotalArea_FLAECHE());
    defineField("VERDUNSTUN", "N", initValues.getDigitsEvaporation_VERDUNSTUN());

    assert(getNumberOfFields() == 9);

    /*
    m_fields.push_back(DbaseField("CODE", "C", 0));
    m_fields.push_back(DbaseField("R", "N", initValues.getDigitsTotalRunoff()));
    m_fields.push_back(DbaseField("ROW", "N", initValues.getDigitsRunoff()));
    m_fields.push_back(DbaseField("RI", "N", initValues.getDigitsInfiltrationRate()));
    m_fields.push_back(DbaseField("RVOL", "N", initValues.getDigitsTotalRunoffFlow()));
    m_fields.push_back(DbaseField("ROWVOL", "N", initValues.getDigitsRainwaterRunoff()));
    m_fields.push_back(DbaseField("RIVOL", "N", initValues.getDigitsTotalSubsurfaceFlow()));
    m_fields.push_back(DbaseField("FLAECHE", "N", initValues.getDigitsTotalArea()));
    m_fields.push_back(DbaseField("VERDUNSTUN", "N", initValues.getDigitsEvaporation()));
    */
}
