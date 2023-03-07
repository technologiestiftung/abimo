#ifndef COUNTERS_H
#define COUNTERS_H

class Counters
{
public:
    Counters();

    void initialise();

    void setRecordsRead(int n);
    void setRecordsWritten(int n);

    void incrementIrrigationForcedToZero();
    void incrementNoAreaGiven();
    void incrementNoUsageGiven();
    void incrementRecordsProtocol();

    int getIrrigationForcedToZero();
    long getNoAreaGiven();
    long getNoUsageGiven();
    long getRecordsProtocol();
    int getRecordsRead();
    int getRecordsWritten();

private:
    // Total records written
    int recordsWritten;

    // Total records read
    int recordsRead;

    // Number of records for which irrigation (BER) was set to zero
    int irrigationForcedToZero;

    // Number of cases in which no calculation was performed
    long noAreaGiven;
    long noUsageGiven;

    // Number of records written to the (error) protocol
    long recordsProtocol;
};

#endif // COUNTERS_H
