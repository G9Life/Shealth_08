#include "HeightImputer.h"

#include "AgeDecadeImputer.h"

void HeightImputer::imputeMissingHeights(std::vector<HealthRecord>& records) {
    AgeDecadeImputer::imputeMissingByAgeDecade(
        records,
        [](const HealthRecord& record) { return record.heightCm; },
        [](HealthRecord& record, double value) { record.heightCm = value; },
        [](const HealthRecord& record) { return record.heightCm == 0.0; });
}
