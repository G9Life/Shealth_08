#include "BmiDistributionAnalyzer.h"

#include <array>

#include "AgeDecadePolicy.h"
#include "BmiCalculator.h"

namespace {

constexpr double kPercentMultiplier = 100.0;

constexpr size_t bmiCategoryIndex(BmiCategory category) {
    switch (category) {
        case BmiCategory::Underweight:
            return 0;
        case BmiCategory::Normal:
            return 1;
        case BmiCategory::Overweight:
            return 2;
        case BmiCategory::Obesity:
            return 3;
    }
    return 0;
}

struct CategoryCounts {
    std::array<int, 4> byCategory{};
};

void incrementCategoryCount(CategoryCounts& counts, BmiCategory category) {
    ++counts.byCategory[bmiCategoryIndex(category)];
}

AgeDecadeDistribution toPercentDistribution(const CategoryCounts& counts, int memberCount) {
    AgeDecadeDistribution distribution;
    if (memberCount == 0) {
        return distribution;
    }

    double* const percentFields[] = {
        &distribution.underweightPercent,
        &distribution.normalPercent,
        &distribution.overweightPercent,
        &distribution.obesityPercent,
    };

    for (size_t categoryIndex = 0; categoryIndex < std::size(percentFields); ++categoryIndex) {
        *percentFields[categoryIndex] =
            static_cast<double>(counts.byCategory[categoryIndex]) * kPercentMultiplier / memberCount;
    }
    return distribution;
}

}  // namespace

void BmiDistributionAnalyzer::calculateBmis(std::vector<HealthRecord>& records) {
    for (HealthRecord& record : records) {
        record.bmi = BmiCalculator::computeBmi(record.weightKg, record.heightCm);
    }
}

double BmiDistributionAnalyzer::percentForCategory(const AgeDecadeDistribution& distribution,
                                                   BmiCategory category) {
    switch (category) {
        case BmiCategory::Underweight:
            return distribution.underweightPercent;
        case BmiCategory::Normal:
            return distribution.normalPercent;
        case BmiCategory::Overweight:
            return distribution.overweightPercent;
        case BmiCategory::Obesity:
            return distribution.obesityPercent;
    }
    return 0.0;
}

AgeDecadeDistribution BmiDistributionAnalyzer::computeDistributionForDecade(
    const std::vector<HealthRecord>& records, int ageDecade) {
    CategoryCounts counts;
    int memberCount = 0;

    for (const HealthRecord& record : records) {
        if (!AgeDecadePolicy::belongsToAgeDecade(record.age, ageDecade)) {
            continue;
        }

        ++memberCount;
        incrementCategoryCount(counts, BmiCalculator::classifyBmi(record.bmi));
    }

    return toPercentDistribution(counts, memberCount);
}

std::unordered_map<int, AgeDecadeDistribution> BmiDistributionAnalyzer::computeDistributionsByAgeDecade(
    const std::vector<HealthRecord>& records) {
    std::unordered_map<int, AgeDecadeDistribution> distributionsByDecade;

    AgeDecadePolicy::forEachAgeDecade([&records, &distributionsByDecade](int ageDecade) {
        distributionsByDecade[ageDecade] = computeDistributionForDecade(records, ageDecade);
    });

    return distributionsByDecade;
}
