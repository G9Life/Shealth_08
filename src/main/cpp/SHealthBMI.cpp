#include "SHealth.h"

#include <cstdio>
#include <string>

namespace {

constexpr const char* kDefaultDataFile = "shealth.dat";

void printAgeDecadeDistribution(const SHealth& analyzer, int ageDecade) {
    printf("%d - underweight = %f, normal = %f, overweight = %f, obesity = %f\n",
           ageDecade,
           analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Underweight),
           analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Normal),
           analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Overweight),
           analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Obesity));
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string requestedPath = (argc > 1) ? argv[1] : kDefaultDataFile;
    const std::string dataFilePath = SHealth::resolveDataFilePath(requestedPath);

    SHealth analyzer;
    analyzer.loadAndCalculate(dataFilePath);

    SHealth::forEachAgeDecade([&analyzer](int ageDecade) {
        printAgeDecadeDistribution(analyzer, ageDecade);
    });

    return 0;
}
