#include "WeightImputer.h"

#include "AgeDecadeImputer.h"

void WeightImputer::imputeMissingWeights(std::vector<HealthRecord>& records) {
    AgeDecadeImputer::imputeMissingByAgeDecade(
        records,
        [](const HealthRecord& record) { return record.weightKg; },
        [](HealthRecord& record, double value) { record.weightKg = value; },
        [](const HealthRecord& record) { return record.weightKg == 0.0; });
}
