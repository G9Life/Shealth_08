#pragma once

#include <array>

enum class BmiCategory {
    Underweight = 100,
    Normal = 200,
    Overweight = 300,
    Obesity = 400
};

inline constexpr std::array<BmiCategory, 4> kAllBmiCategories = {
    BmiCategory::Underweight,
    BmiCategory::Normal,
    BmiCategory::Overweight,
    BmiCategory::Obesity,
};

struct AgeDecadeDistribution {
    double underweightPercent = 0.0;
    double normalPercent = 0.0;
    double overweightPercent = 0.0;
    double obesityPercent = 0.0;
};
