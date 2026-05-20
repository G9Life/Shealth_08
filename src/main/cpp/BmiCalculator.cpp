#include "BmiCalculator.h"

namespace {

constexpr double kCmPerMeter = 100.0;

}  // namespace

double BmiCalculator::computeBmi(double weightKg, double heightCm) {
    if (heightCm == 0.0) {
        return 0.0;
    }
    const double heightMeters = heightCm / kCmPerMeter;
    return weightKg / (heightMeters * heightMeters);
}

BmiCategory BmiCalculator::classifyBmi(double bmi) {
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
