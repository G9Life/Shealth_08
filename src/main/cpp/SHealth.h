#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include "AgeDecadePolicy.h"
#include "BmiTypes.h"
#include "HealthRecord.h"

class SHealth {
public:
    static constexpr int kMinAgeDecade = AgeDecadePolicy::kMinAgeDecade;
    static constexpr int kMaxAgeDecade = AgeDecadePolicy::kMaxAgeDecade;
    static constexpr int kAgeDecadeSpan = AgeDecadePolicy::kAgeDecadeSpan;

    using BmiCategory = ::BmiCategory;
    static constexpr std::array<BmiCategory, 4> kAllBmiCategories = ::kAllBmiCategories;

    using HealthRecord = ::HealthRecord;
    using AgeDecadeDistribution = ::AgeDecadeDistribution;

    size_t loadAndCalculate(const std::string& filename);
    double getCategoryPercent(int ageDecade, BmiCategory category) const;
    double getCategoryPercent(int ageDecade, int categoryCode) const;
    double getBmiRatio(int ageDecade, int categoryCode) const;
    double sumCategoryPercents(int ageDecade) const;

    static bool isValidRecord(int id, int age, double heightCm);
    static BmiCategory classifyBmi(double bmi);
    static double computeBmi(double weightKg, double heightCm);
    static bool belongsToAgeDecade(int age, int ageDecade);
    static std::string resolveDataFilePath(const std::string& preferredPath);

    template <typename Callback>
    static void forEachAgeDecade(Callback&& callback) {
        AgeDecadePolicy::forEachAgeDecade(std::forward<Callback>(callback));
    }

private:
    std::vector<HealthRecord> records_;
    std::unordered_map<int, AgeDecadeDistribution> distributionsByDecade_;
};
