#include "WeightImputer.h"

#include "AgeDecadePolicy.h"

double WeightImputer::averageWeightForDecade(const std::vector<HealthRecord>& records,
                                             int ageDecade) {
    double weightSum = 0.0;
    int validWeightCount = 0;

    for (const HealthRecord& record : records) {
        if (!AgeDecadePolicy::belongsToAgeDecade(record.age, ageDecade) || record.weightKg == 0.0) {
            continue;
        }
        weightSum += record.weightKg;
        ++validWeightCount;
    }

    if (validWeightCount == 0) {
        return 0.0;
    }
    return weightSum / validWeightCount;
}

void WeightImputer::imputeMissingWeights(std::vector<HealthRecord>& records) {
    AgeDecadePolicy::forEachAgeDecade([&records](int ageDecade) {
        const double averageWeight = averageWeightForDecade(records, ageDecade);
        if (averageWeight == 0.0) {
            return;
        }

        for (HealthRecord& record : records) {
            if (AgeDecadePolicy::belongsToAgeDecade(record.age, ageDecade) && record.weightKg == 0.0) {
                record.weightKg = averageWeight;
            }
        }
    });
}
