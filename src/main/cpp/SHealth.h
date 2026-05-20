#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

class SHealth {
public:
    static constexpr int kMinAgeDecade = 20;
    static constexpr int kMaxAgeDecade = 70;
    static constexpr int kAgeDecadeSpan = 10;

    enum class BmiCategory {
        Underweight = 100,
        Normal = 200,
        Overweight = 300,
        Obesity = 400
    };

    static constexpr std::array<BmiCategory, 4> kAllBmiCategories = {
        BmiCategory::Underweight,
        BmiCategory::Normal,
        BmiCategory::Overweight,
        BmiCategory::Obesity,
    };

    struct HealthRecord {
        int id = 0;
        int age = 0;
        double weightKg = 0.0;
        double heightCm = 0.0;
        double bmi = 0.0;
    };

    struct AgeDecadeDistribution {
        double underweightPercent = 0.0;
        double normalPercent = 0.0;
        double overweightPercent = 0.0;
        double obesityPercent = 0.0;
    };

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
        for (int ageDecade = kMinAgeDecade; ageDecade <= kMaxAgeDecade; ageDecade += kAgeDecadeSpan) {
            callback(ageDecade);
        }
    }

private:
    static constexpr double kUnderweightMaxBmi = 18.5;
    static constexpr double kNormalMaxBmi = 23.0;
    static constexpr double kOverweightMaxBmi = 25.0;

    std::vector<HealthRecord> records_;
    std::unordered_map<int, AgeDecadeDistribution> distributionsByDecade_;

    bool loadRecordsFromFile(const std::string& filename);
    void imputeMissingWeightsByAgeDecade();
    void calculateBmis();
    void calculateDistributionsByAgeDecade();

    double averageWeightForDecade(int ageDecade) const;
    AgeDecadeDistribution computeDistributionForDecade(int ageDecade) const;

    static std::vector<std::string> splitCsvLine(const std::string& line, char delimiter);
};
