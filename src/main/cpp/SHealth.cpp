#include "SHealth.h"

#include "BmiCalculator.h"
#include "BmiDistributionAnalyzer.h"
#include "HealthRecordCsvReader.h"
#include "WeightImputer.h"

bool SHealth::isValidRecord(int id, int age, double heightCm) {
    return HealthRecordCsvReader::isValidRecord(id, age, heightCm);
}

bool SHealth::belongsToAgeDecade(int age, int ageDecade) {
    return AgeDecadePolicy::belongsToAgeDecade(age, ageDecade);
}

double SHealth::computeBmi(double weightKg, double heightCm) {
    return BmiCalculator::computeBmi(weightKg, heightCm);
}

SHealth::BmiCategory SHealth::classifyBmi(double bmi) {
    return BmiCalculator::classifyBmi(bmi);
}

std::string SHealth::resolveDataFilePath(const std::string& preferredPath) {
    return HealthRecordCsvReader::resolveDataFilePath(preferredPath);
}

size_t SHealth::loadAndCalculate(const std::string& filename) {
    if (!HealthRecordCsvReader::loadFromFile(filename, records_)) {
        return 0;
    }

    WeightImputer::imputeMissingWeights(records_);
    BmiDistributionAnalyzer::calculateBmis(records_);
    distributionsByDecade_ = BmiDistributionAnalyzer::computeDistributionsByAgeDecade(records_);

    return records_.size();
}

double SHealth::getCategoryPercent(int ageDecade, BmiCategory category) const {
    const auto distributionIt = distributionsByDecade_.find(ageDecade);
    if (distributionIt == distributionsByDecade_.end()) {
        return 0.0;
    }
    return BmiDistributionAnalyzer::percentForCategory(distributionIt->second, category);
}

double SHealth::getCategoryPercent(int ageDecade, int categoryCode) const {
    return getCategoryPercent(ageDecade, static_cast<BmiCategory>(categoryCode));
}

double SHealth::getBmiRatio(int ageDecade, int categoryCode) const {
    return getCategoryPercent(ageDecade, categoryCode);
}

SHealth::AgeDecadeDistribution SHealth::getDistributionForAgeDecade(int ageDecade) const {
    const auto distributionIt = distributionsByDecade_.find(ageDecade);
    if (distributionIt == distributionsByDecade_.end()) {
        return AgeDecadeDistribution{};
    }
    return distributionIt->second;
}

double SHealth::sumCategoryPercents(int ageDecade) const {
    double total = 0.0;
    for (BmiCategory category : kAllBmiCategories) {
        total += getCategoryPercent(ageDecade, category);
    }
    return total;
}
