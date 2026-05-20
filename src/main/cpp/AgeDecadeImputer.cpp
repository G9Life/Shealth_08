#include "AgeDecadeImputer.h"

#include "AgeDecadePolicy.h"

double AgeDecadeImputer::averageForDecade(const std::vector<HealthRecord>& records,
                                          int ageDecade,
                                          const GetValue& getValue,
                                          const IsMissing& isMissing) {
    double valueSum = 0.0;
    int validValueCount = 0;

    for (const HealthRecord& record : records) {
        if (!AgeDecadePolicy::belongsToAgeDecade(record.age, ageDecade) || isMissing(record)) {
            continue;
        }
        valueSum += getValue(record);
        ++validValueCount;
    }

    if (validValueCount == 0) {
        return 0.0;
    }
    return valueSum / validValueCount;
}

void AgeDecadeImputer::imputeMissingByAgeDecade(std::vector<HealthRecord>& records,
                                                const GetValue& getValue,
                                                const SetValue& setValue,
                                                const IsMissing& isMissing) {
    AgeDecadePolicy::forEachAgeDecade([&records, &getValue, &setValue, &isMissing](int ageDecade) {
        const double averageValue = averageForDecade(records, ageDecade, getValue, isMissing);
        if (averageValue == 0.0) {
            return;
        }

        for (HealthRecord& record : records) {
            if (AgeDecadePolicy::belongsToAgeDecade(record.age, ageDecade) && isMissing(record)) {
                setValue(record, averageValue);
            }
        }
    });
}
