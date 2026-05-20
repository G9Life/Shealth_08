#pragma once

#include <unordered_map>
#include <vector>

#include "BmiTypes.h"
#include "HealthRecord.h"

class BmiDistributionAnalyzer {
public:
    static void calculateBmis(std::vector<HealthRecord>& records);
    static std::unordered_map<int, AgeDecadeDistribution> computeDistributionsByAgeDecade(
        const std::vector<HealthRecord>& records);

    static double percentForCategory(const AgeDecadeDistribution& distribution, BmiCategory category);

private:
    static AgeDecadeDistribution computeDistributionForDecade(
        const std::vector<HealthRecord>& records, int ageDecade);
};
