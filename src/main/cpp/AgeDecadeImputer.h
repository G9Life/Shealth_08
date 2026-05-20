#pragma once

#include <functional>
#include <vector>

#include "HealthRecord.h"

class AgeDecadeImputer {
public:
    using GetValue = std::function<double(const HealthRecord&)>;
    using SetValue = std::function<void(HealthRecord&, double)>;
    using IsMissing = std::function<bool(const HealthRecord&)>;

    static double averageForDecade(const std::vector<HealthRecord>& records,
                                   int ageDecade,
                                   const GetValue& getValue,
                                   const IsMissing& isMissing);

    static void imputeMissingByAgeDecade(std::vector<HealthRecord>& records,
                                         const GetValue& getValue,
                                         const SetValue& setValue,
                                         const IsMissing& isMissing);
};
