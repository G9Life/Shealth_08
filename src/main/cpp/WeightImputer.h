#pragma once

#include <vector>

#include "HealthRecord.h"

class WeightImputer {
public:
    static void imputeMissingWeights(std::vector<HealthRecord>& records);

private:
    static double averageWeightForDecade(const std::vector<HealthRecord>& records, int ageDecade);
};
