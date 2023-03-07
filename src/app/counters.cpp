#include "counters.h"

Counters::Counters()
{
    initialise();
}

void Counters::initialise() {
    recordsWritten = 0;
    recordsRead = 0;
    irrigationForcedToZero = 0;
    noAreaGiven = 0L;
    noUsageGiven = 0L;
    recordsProtocol = 0L;
}

void Counters::setRecordsRead(int n) {
    recordsRead = n;
}

void Counters::setRecordsWritten(int n) {
    recordsWritten = n;
}

void Counters::incrementIrrigationForcedToZero() {
    irrigationForcedToZero++;
}

void Counters::incrementNoAreaGiven() {
    noAreaGiven++;
}

void Counters::incrementNoUsageGiven() {
    noUsageGiven++;
}

void Counters::incrementRecordsProtocol() {
    recordsProtocol++;
}

int Counters::getIrrigationForcedToZero() {
    return irrigationForcedToZero;
}

long Counters::getNoAreaGiven() {
    return noAreaGiven;
}

long Counters::getNoUsageGiven() {
    return noUsageGiven;
}

long Counters::getRecordsProtocol() {
    return recordsProtocol;
}

int Counters::getRecordsRead() {
    return recordsRead;
}

int Counters::getRecordsWritten() {
    return recordsWritten;
}
