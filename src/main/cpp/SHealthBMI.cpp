#include "SHealth.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

constexpr const char* kDefaultDataFile = "shealth.dat";

void printAgeDecadeDistribution(const SHealth& analyzer, int ageDecade) {
    std::cout << ageDecade << " - underweight = "
              << analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Underweight)
              << ", normal = "
              << analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Normal)
              << ", overweight = "
              << analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Overweight)
              << ", obesity = "
              << analyzer.getCategoryPercent(ageDecade, SHealth::BmiCategory::Obesity) << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string requestedPath = (argc > 1) ? argv[1] : kDefaultDataFile;
    const std::string dataFilePath = SHealth::resolveDataFilePath(requestedPath);

    SHealth analyzer;
    const size_t recordCount = analyzer.loadAndCalculate(dataFilePath);
    if (recordCount == 0) {
        std::cerr << "Failed to load health records from: " << dataFilePath << std::endl;
        return EXIT_FAILURE;
    }

    SHealth::forEachAgeDecade([&analyzer](int ageDecade) {
        printAgeDecadeDistribution(analyzer, ageDecade);
    });

    return EXIT_SUCCESS;
}
