#include "SHealth.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

constexpr double kCmPerMeter = 100.0;
constexpr double kPercentMultiplier = 100.0;

constexpr size_t bmiCategoryIndex(SHealth::BmiCategory category) {
    switch (category) {
        case SHealth::BmiCategory::Underweight:
            return 0;
        case SHealth::BmiCategory::Normal:
            return 1;
        case SHealth::BmiCategory::Overweight:
            return 2;
        case SHealth::BmiCategory::Obesity:
            return 3;
    }
    return 0;
}

struct CategoryCounts {
    std::array<int, 4> byCategory{};
};

void incrementCategoryCount(CategoryCounts& counts, SHealth::BmiCategory category) {
    ++counts.byCategory[bmiCategoryIndex(category)];
}

double percentForCategory(const SHealth::AgeDecadeDistribution& distribution,
                          SHealth::BmiCategory category) {
    switch (category) {
        case SHealth::BmiCategory::Underweight:
            return distribution.underweightPercent;
        case SHealth::BmiCategory::Normal:
            return distribution.normalPercent;
        case SHealth::BmiCategory::Overweight:
            return distribution.overweightPercent;
        case SHealth::BmiCategory::Obesity:
            return distribution.obesityPercent;
    }
    return 0.0;
}

SHealth::AgeDecadeDistribution toPercentDistribution(const CategoryCounts& counts, int memberCount) {
    SHealth::AgeDecadeDistribution distribution;
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

bool SHealth::isValidRecord(int id, int age, double heightCm) {
    return id >= 0 && age > 0 && heightCm >= 0.0;
}

bool SHealth::belongsToAgeDecade(int age, int ageDecade) {
    return age >= ageDecade && age < ageDecade + kAgeDecadeSpan;
}

double SHealth::computeBmi(double weightKg, double heightCm) {
    if (heightCm == 0.0) {
        return 0.0;
    }
    const double heightMeters = heightCm / kCmPerMeter;
    return weightKg / (heightMeters * heightMeters);
}

SHealth::BmiCategory SHealth::classifyBmi(double bmi) {
    if (bmi <= kUnderweightMaxBmi) {
        return BmiCategory::Underweight;
    }
    if (bmi < kNormalMaxBmi) {
        return BmiCategory::Normal;
    }
    if (bmi < kOverweightMaxBmi) {
        return BmiCategory::Overweight;
    }
    return BmiCategory::Obesity;
}

std::string SHealth::resolveDataFilePath(const std::string& preferredPath) {
    std::ifstream file(preferredPath);
    if (file.good()) {
        return preferredPath;
    }

    const std::string fallbackPath = std::string("../") + preferredPath;
    std::ifstream fallbackFile(fallbackPath);
    if (fallbackFile.good()) {
        return fallbackPath;
    }

    return preferredPath;
}

std::vector<std::string> SHealth::splitCsvLine(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool SHealth::loadRecordsFromFile(const std::string& filename) {
    records_.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);  // header

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> fields = splitCsvLine(line, ',');
        if (fields.size() < 4) {
            continue;
        }

        const int id = std::stoi(fields[0]);
        const int age = std::stoi(fields[1]);
        const double weightKg = std::stod(fields[2]);
        const double heightCm = std::stod(fields[3]);

        if (!isValidRecord(id, age, heightCm)) {
            continue;
        }

        records_.push_back({id, age, weightKg, heightCm, 0.0});
    }

    return true;
}

double SHealth::averageWeightForDecade(int ageDecade) const {
    double weightSum = 0.0;
    int validWeightCount = 0;

    for (const HealthRecord& record : records_) {
        if (!belongsToAgeDecade(record.age, ageDecade) || record.weightKg == 0.0) {
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

void SHealth::imputeMissingWeightsByAgeDecade() {
    forEachAgeDecade([this](int ageDecade) {
        const double averageWeight = averageWeightForDecade(ageDecade);
        if (averageWeight == 0.0) {
            return;
        }

        for (HealthRecord& record : records_) {
            if (belongsToAgeDecade(record.age, ageDecade) && record.weightKg == 0.0) {
                record.weightKg = averageWeight;
            }
        }
    });
}

void SHealth::calculateBmis() {
    for (HealthRecord& record : records_) {
        record.bmi = computeBmi(record.weightKg, record.heightCm);
    }
}

SHealth::AgeDecadeDistribution SHealth::computeDistributionForDecade(int ageDecade) const {
    CategoryCounts counts;
    int memberCount = 0;

    for (const HealthRecord& record : records_) {
        if (!belongsToAgeDecade(record.age, ageDecade)) {
            continue;
        }

        ++memberCount;
        incrementCategoryCount(counts, classifyBmi(record.bmi));
    }

    return toPercentDistribution(counts, memberCount);
}

void SHealth::calculateDistributionsByAgeDecade() {
    distributionsByDecade_.clear();

    forEachAgeDecade([this](int ageDecade) {
        distributionsByDecade_[ageDecade] = computeDistributionForDecade(ageDecade);
    });
}

size_t SHealth::loadAndCalculate(const std::string& filename) {
    if (!loadRecordsFromFile(filename)) {
        return 0;
    }

    imputeMissingWeightsByAgeDecade();
    calculateBmis();
    calculateDistributionsByAgeDecade();

    return records_.size();
}

double SHealth::getCategoryPercent(int ageDecade, BmiCategory category) const {
    const auto distributionIt = distributionsByDecade_.find(ageDecade);
    if (distributionIt == distributionsByDecade_.end()) {
        return 0.0;
    }
    return percentForCategory(distributionIt->second, category);
}

double SHealth::getCategoryPercent(int ageDecade, int categoryCode) const {
    return getCategoryPercent(ageDecade, static_cast<BmiCategory>(categoryCode));
}

double SHealth::getBmiRatio(int ageDecade, int categoryCode) const {
    return getCategoryPercent(ageDecade, categoryCode);
}

double SHealth::sumCategoryPercents(int ageDecade) const {
    double total = 0.0;
    for (BmiCategory category : kAllBmiCategories) {
        total += getCategoryPercent(ageDecade, category);
    }
    return total;
}
