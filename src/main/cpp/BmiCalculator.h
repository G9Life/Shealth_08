#pragma once

#include "BmiTypes.h"

class BmiCalculator {
public:
    static double computeBmi(double weightKg, double heightCm);
    static BmiCategory classifyBmi(double bmi);

private:
    static constexpr double kUnderweightMaxBmi = 18.5;
    static constexpr double kNormalMaxBmi = 23.0;
    static constexpr double kOverweightMaxBmi = 25.0;
};
