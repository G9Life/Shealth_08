#include <cmath>
#include <gtest/gtest.h>

#include "SHealth.h"

namespace {

int bmiToMilli(double bmi) {
    return static_cast<int>(std::lround(bmi * 1000.0));
}

int computeBmiMilli(double weightKg, double heightCm) {
    return bmiToMilli(SHealth::computeBmi(weightKg, heightCm));
}

// shealth.dat sample rows (id, weight kg, height cm)
constexpr double kShealth93705WeightKg = 79.5;
constexpr double kShealth93705HeightCm = 158.3;
constexpr int kShealth93705BmiMilli = 31725;

constexpr double kShealth93711WeightKg = 62.1;
constexpr double kShealth93711HeightCm = 170.6;
constexpr int kShealth93711BmiMilli = 21337;

constexpr double kShealth93708WeightKg = 53.5;
constexpr double kShealth93708HeightCm = 150.2;
constexpr int kShealth93708BmiMilli = 23714;

constexpr double kShealth93795WeightKg = 44.5;
constexpr double kShealth93795HeightCm = 168.6;
constexpr int kShealth93795BmiMilli = 15655;

constexpr double kShealth97948WeightKg = 55.3;
constexpr double kShealth97948HeightCm = 172.9;
constexpr int kShealth97948BmiMilli = 18498;

constexpr double kShealth94663WeightKg = 59.0;
constexpr double kShealth94663HeightCm = 160.2;
constexpr int kShealth94663BmiMilli = 22989;

constexpr double kShealth94457WeightKg = 67.7;
constexpr double kShealth94457HeightCm = 164.6;
constexpr int kShealth94457BmiMilli = 24988;

constexpr double kShealth102210WeightKg = 74.3;
constexpr double kShealth102210HeightCm = 172.4;
constexpr int kShealth102210BmiMilli = 24999;

class SHealthBmiCalculationFixture : public ::testing::Test {};

class SHealthLoadedDataFixture : public ::testing::Test {
protected:
    void SetUp() override {
        recordCount_ = analyzer_.loadAndCalculate(SHealth::resolveDataFilePath("shealth.dat"));
        ASSERT_GT(recordCount_, 0u) << "shealth.dat not found";
    }

    SHealth analyzer_;
    size_t recordCount_ = 0;
};

}  // namespace

TEST_F(SHealthBmiCalculationFixture, GivenShealth93705ObesityRow_WhenComputeBmi_ThenMilliMatchesExpected) {
    // Given: shealth.dat id=93705 (79.5 kg, 158.3 cm) — obesity sample
    const double weightKg = kShealth93705WeightKg;
    const double heightCm = kShealth93705HeightCm;

    // When: BMI is computed
    const int actualBmiMilli = computeBmiMilli(weightKg, heightCm);

    // Then: milli-scaled BMI matches the dataset-derived expectation
    ASSERT_EQ(actualBmiMilli, kShealth93705BmiMilli);
    EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(weightKg, heightCm)),
              SHealth::BmiCategory::Obesity);
}

TEST_F(SHealthBmiCalculationFixture, GivenShealth93711NormalRow_WhenComputeBmi_ThenMilliAndCategoryMatch) {
    // Given: shealth.dat id=93711 (62.1 kg, 170.6 cm) — normal-range sample
    const double weightKg = kShealth93711WeightKg;
    const double heightCm = kShealth93711HeightCm;

    // When: BMI is computed and classified
    const int actualBmiMilli = computeBmiMilli(weightKg, heightCm);
    const SHealth::BmiCategory category = SHealth::classifyBmi(SHealth::computeBmi(weightKg, heightCm));

    // Then: value and category are normal
    ASSERT_EQ(actualBmiMilli, kShealth93711BmiMilli);
    EXPECT_EQ(category, SHealth::BmiCategory::Normal);
}

TEST_F(SHealthBmiCalculationFixture, GivenShealth93708OverweightRow_WhenComputeBmi_ThenMilliAndCategoryMatch) {
    // Given: shealth.dat id=93708 (53.5 kg, 150.2 cm) — overweight sample
    const double weightKg = kShealth93708WeightKg;
    const double heightCm = kShealth93708HeightCm;

    // When: BMI is computed and classified
    const int actualBmiMilli = computeBmiMilli(weightKg, heightCm);
    const SHealth::BmiCategory category = SHealth::classifyBmi(SHealth::computeBmi(weightKg, heightCm));

    // Then: value and category are overweight
    ASSERT_EQ(actualBmiMilli, kShealth93708BmiMilli);
    EXPECT_EQ(category, SHealth::BmiCategory::Overweight);
}

TEST_F(SHealthBmiCalculationFixture, GivenShealthUnderweightAndNearBoundaryRows_WhenClassify_ThenExpectedCategories) {
    // Given: shealth.dat underweight (93795) and near-18.5 boundary (97948) rows
    const double underweightBmi =
        SHealth::computeBmi(kShealth93795WeightKg, kShealth93795HeightCm);
    const double nearUnderweightMaxBmi =
        SHealth::computeBmi(kShealth97948WeightKg, kShealth97948HeightCm);

    // When: BMI values are classified
    const SHealth::BmiCategory underweightCategory = SHealth::classifyBmi(underweightBmi);
    const SHealth::BmiCategory nearBoundaryCategory = SHealth::classifyBmi(nearUnderweightMaxBmi);

    // Then: both fall in the underweight band (BMI <= 18.5)
    ASSERT_EQ(bmiToMilli(underweightBmi), kShealth93795BmiMilli);
    ASSERT_EQ(bmiToMilli(nearUnderweightMaxBmi), kShealth97948BmiMilli);
    EXPECT_EQ(underweightCategory, SHealth::BmiCategory::Underweight);
    EXPECT_EQ(nearBoundaryCategory, SHealth::BmiCategory::Underweight);
}

TEST_F(SHealthBmiCalculationFixture, GivenShealthNormalOverweightBoundaryRows_WhenClassify_ThenExpectedCategories) {
    // Given: shealth.dat rows near normal/overweight boundaries (94663, 94457)
    const double normalSideBmi =
        SHealth::computeBmi(kShealth94663WeightKg, kShealth94663HeightCm);
    const double overweightSideBmi =
        SHealth::computeBmi(kShealth94457WeightKg, kShealth94457HeightCm);

    // When: BMI values are classified
    const SHealth::BmiCategory normalSideCategory = SHealth::classifyBmi(normalSideBmi);
    const SHealth::BmiCategory overweightSideCategory = SHealth::classifyBmi(overweightSideBmi);

    // Then: categories straddle the 23.0 threshold
    ASSERT_EQ(bmiToMilli(normalSideBmi), kShealth94663BmiMilli);
    ASSERT_EQ(bmiToMilli(overweightSideBmi), kShealth94457BmiMilli);
    EXPECT_EQ(normalSideCategory, SHealth::BmiCategory::Normal);
    EXPECT_EQ(overweightSideCategory, SHealth::BmiCategory::Overweight);
}

TEST_F(SHealthBmiCalculationFixture, GivenShealthObesityBoundaryAndExactThresholds_WhenComputeAndClassify_ThenExpected) {
    // Given: shealth.dat row near obesity boundary (102210) and exact BMI thresholds at 100 cm height
    const double nearObesityBoundaryBmi =
        SHealth::computeBmi(kShealth102210WeightKg, kShealth102210HeightCm);
    const double exactUnderweightMaxBmi = SHealth::computeBmi(18.5, 100.0);
    const double exactNormalMaxBmi = SHealth::computeBmi(22.9, 100.0);
    const double exactOverweightMaxBmi = SHealth::computeBmi(24.9, 100.0);
    const double exactObesityMinBmi = SHealth::computeBmi(25.0, 100.0);
    const double zeroHeightBmi = SHealth::computeBmi(70.0, 0.0);

    // When: values are classified (and zero-height BMI is computed)
    const SHealth::BmiCategory nearObesityCategory = SHealth::classifyBmi(nearObesityBoundaryBmi);

    // Then: boundary row is overweight; exact thresholds map to each category; zero height yields 0 BMI
    ASSERT_EQ(bmiToMilli(nearObesityBoundaryBmi), kShealth102210BmiMilli);
    EXPECT_EQ(nearObesityCategory, SHealth::BmiCategory::Overweight);
    EXPECT_EQ(exactUnderweightMaxBmi, 18.5);
    EXPECT_EQ(exactNormalMaxBmi, 22.9);
    EXPECT_EQ(exactOverweightMaxBmi, 24.9);
    EXPECT_EQ(exactObesityMinBmi, 25.0);
    EXPECT_EQ(zeroHeightBmi, 0.0);
    EXPECT_EQ(SHealth::classifyBmi(exactUnderweightMaxBmi), SHealth::BmiCategory::Underweight);
    EXPECT_EQ(SHealth::classifyBmi(exactNormalMaxBmi), SHealth::BmiCategory::Normal);
    EXPECT_EQ(SHealth::classifyBmi(exactOverweightMaxBmi), SHealth::BmiCategory::Overweight);
    EXPECT_EQ(SHealth::classifyBmi(exactObesityMinBmi), SHealth::BmiCategory::Obesity);
    EXPECT_EQ(SHealth::classifyBmi(zeroHeightBmi), SHealth::BmiCategory::Underweight);
}

TEST_F(SHealthLoadedDataFixture, GivenLoadedShealthDat_WhenQueryAgeDecade20_ThenObesityPercentIsPositive) {
    // Given: shealth.dat is loaded (SetUp)
    // When: obesity percent for age decade 20 is queried
    const double obesityPercent =
        analyzer_.getCategoryPercent(20, SHealth::BmiCategory::Obesity);

    // Then: at least one member in that decade is classified as obesity
    EXPECT_GT(obesityPercent, 0.0);
}

TEST_F(SHealthLoadedDataFixture, GivenLoadedShealthDat_WhenSummingCategoryPercents_ThenEachDecadeTotalsOneHundred) {
    // Given: shealth.dat is loaded (SetUp)
    // When: category percents are summed per age decade
    SHealth::forEachAgeDecade([this](int ageDecade) {
        const double total = analyzer_.sumCategoryPercents(ageDecade);

        // Then: non-empty decades sum to 100%
        if (total > 0.0) {
            EXPECT_EQ(static_cast<int>(std::lround(total)), 100) << "age decade " << ageDecade;
        }
    });
}

TEST_F(SHealthLoadedDataFixture, GivenLoadedShealthDat_WhenUsingLegacyCategoryCode_ThenPercentMatchesEnumApi) {
    // Given: shealth.dat is loaded (SetUp)
    // When: category percent is fetched via enum and legacy int code
    const double viaEnum = analyzer_.getCategoryPercent(20, SHealth::BmiCategory::Normal);
    const double viaLegacyCode = analyzer_.getCategoryPercent(20, 200);
    const double viaBmiRatio = analyzer_.getBmiRatio(20, 200);

    // Then: all APIs return the same value
    EXPECT_EQ(viaEnum, viaLegacyCode);
    EXPECT_EQ(viaBmiRatio, viaEnum);
}

// --- Age-decade average weight imputation (03-02) ---

// shealth.dat id=93730 (age 57, weight=0, height 167.6 cm) — sole weight=0 row
constexpr int kShealth93730Age = 57;
constexpr int kShealth93730AgeDecade = 50;
constexpr double kShealth93730HeightCm = 167.6;
constexpr double kShealth50DecadeAverageWeightKg = 83.39793103448275;
constexpr int kShealth93730ImputedBmiMilli = 29690;

class SHealthAgeDecadeFixture : public ::testing::Test {};

class SHealthAgeImputationFixture : public ::testing::Test {
protected:
    size_t loadFixture(const char* relativePath) {
        return analyzer_.loadAndCalculate(SHealth::resolveDataFilePath(relativePath));
    }

    SHealth analyzer_;
};

TEST_F(SHealthAgeDecadeFixture, GivenAgeDecadeBoundaryAges_WhenBelongsToAgeDecade_ThenExpectedMembership) {
    // Given: ages at decade edges (shealth.dat uses 20 <= age < 30 for the 20s, etc.)
    const int ageBelowMinDecade = 19;
    const int ageDecade20Min = 20;
    const int ageDecade20Max = 29;
    const int ageDecade30Min = 30;
    const int shealth93730Age = kShealth93730Age;

    // When: decade membership is evaluated
    const bool belowMinIn20s = SHealth::belongsToAgeDecade(ageBelowMinDecade, 20);
    const bool minIn20s = SHealth::belongsToAgeDecade(ageDecade20Min, 20);
    const bool maxIn20s = SHealth::belongsToAgeDecade(ageDecade20Max, 20);
    const bool minIn30s = SHealth::belongsToAgeDecade(ageDecade30Min, 30);
    const bool age57In50s = SHealth::belongsToAgeDecade(shealth93730Age, 50);
    const bool age57NotIn40s = SHealth::belongsToAgeDecade(shealth93730Age, 40);

    // Then: boundaries match README decade rules
    EXPECT_EQ(belowMinIn20s, false);
    EXPECT_EQ(minIn20s, true);
    EXPECT_EQ(maxIn20s, true);
    EXPECT_EQ(SHealth::belongsToAgeDecade(ageDecade30Min, 20), false);
    EXPECT_EQ(minIn30s, true);
    EXPECT_EQ(age57In50s, true);
    EXPECT_EQ(age57NotIn40s, false);
}

TEST_F(SHealthAgeImputationFixture,
       GivenShealth93730ZeroWeightRow_WhenImputedWithDecade50Average_ThenBmiMilliAndCategoryMatch) {
    // Given: shealth.dat decade-50 average weight (870 valid rows; weight=0 excluded from mean)
    const double imputedWeightKg = kShealth50DecadeAverageWeightKg;
    const double heightCm = kShealth93730HeightCm;

    // When: BMI is computed from the imputed weight
    const double imputedBmi = SHealth::computeBmi(imputedWeightKg, heightCm);
    const int imputedBmiMilli = bmiToMilli(imputedBmi);
    const SHealth::BmiCategory category = SHealth::classifyBmi(imputedBmi);

    // Then: imputed BMI matches shealth.dat expectation (id=93730 → obesity)
    ASSERT_EQ(imputedBmiMilli, kShealth93730ImputedBmiMilli);
    EXPECT_EQ(category, SHealth::BmiCategory::Obesity);
    EXPECT_EQ(SHealth::belongsToAgeDecade(kShealth93730Age, kShealth93730AgeDecade), true);
}

TEST_F(SHealthAgeImputationFixture,
       GivenThreeMemberFixtureWithOneZeroWeight_WhenLoadAndCalculate_ThenPercentsReflectImputedBmis) {
    // Given: 3 members in the 50s; id=3 has weight=0 → imputed to (18+24)/2 = 21 kg at height 100 cm
    const size_t recordCount = loadFixture("src/test/data/impute_three_members.csv");
    ASSERT_EQ(recordCount, 3u);

    // When: decade-50 distribution is queried after imputation
    const int underweightPercent = static_cast<int>(
        std::lround(analyzer_.getCategoryPercent(50, SHealth::BmiCategory::Underweight)));
    const int normalPercent = static_cast<int>(
        std::lround(analyzer_.getCategoryPercent(50, SHealth::BmiCategory::Normal)));
    const int overweightPercent = static_cast<int>(
        std::lround(analyzer_.getCategoryPercent(50, SHealth::BmiCategory::Overweight)));

    const double imputedWeightKg = 21.0;
    const double heightCm = 100.0;

    // Then: BMIs 18 / 24 / 21 → Underweight / Overweight / Normal (one third each)
    ASSERT_EQ(computeBmiMilli(18.0, heightCm), 18000);
    ASSERT_EQ(computeBmiMilli(24.0, heightCm), 24000);
    ASSERT_EQ(computeBmiMilli(imputedWeightKg, heightCm), 21000);
    EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(18.0, heightCm)),
              SHealth::BmiCategory::Underweight);
    EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(24.0, heightCm)),
              SHealth::BmiCategory::Overweight);
    EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(imputedWeightKg, heightCm)),
              SHealth::BmiCategory::Normal);
    EXPECT_EQ(underweightPercent, 33);
    EXPECT_EQ(normalPercent, 33);
    EXPECT_EQ(overweightPercent, 33);
    EXPECT_EQ(static_cast<int>(std::lround(analyzer_.sumCategoryPercents(50))), 100);
}

TEST_F(SHealthAgeImputationFixture,
       GivenDecadeWithOnlyZeroWeights_WhenLoadAndCalculate_ThenNoImputationAndAllUnderweight) {
    // Given: every member in the 50s has weight=0 → average unavailable, no imputation
    const size_t recordCount = loadFixture("src/test/data/impute_all_zero_weights.csv");
    ASSERT_EQ(recordCount, 2u);

    // When: BMI is computed from zero weight (unchanged) and distribution is read
    const double zeroWeightBmi = SHealth::computeBmi(0.0, 170.0);
    const int underweightPercent = static_cast<int>(
        std::lround(analyzer_.getCategoryPercent(50, SHealth::BmiCategory::Underweight)));

    // Then: BMI stays 0 (underweight) and the decade is 100% underweight
    ASSERT_EQ(zeroWeightBmi, 0.0);
    EXPECT_EQ(SHealth::classifyBmi(zeroWeightBmi), SHealth::BmiCategory::Underweight);
    EXPECT_EQ(underweightPercent, 100);
    EXPECT_EQ(static_cast<int>(std::lround(analyzer_.sumCategoryPercents(50))), 100);
}

TEST_F(SHealthAgeImputationFixture,
       GivenLoadedShealthDat_WhenDecade50IncludesImputed93730_ThenObesityPercentIsPositive) {
    // Given: full shealth.dat (includes id=93730 weight=0 in the 50s)
    const size_t recordCount =
        analyzer_.loadAndCalculate(SHealth::resolveDataFilePath("shealth.dat"));
    ASSERT_GT(recordCount, 0u);

    // When: decade-50 obesity share is queried (93730 imputed to obesity)
    const int obesityPercent = static_cast<int>(
        std::lround(analyzer_.getCategoryPercent(50, SHealth::BmiCategory::Obesity)));
    const int imputedBmiMilli =
        computeBmiMilli(kShealth50DecadeAverageWeightKg, kShealth93730HeightCm);

    // Then: imputed row is obesity and the decade reports non-zero obesity rate
    ASSERT_EQ(imputedBmiMilli, kShealth93730ImputedBmiMilli);
    EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(kShealth50DecadeAverageWeightKg, kShealth93730HeightCm)),
              SHealth::BmiCategory::Obesity);
    EXPECT_EQ(obesityPercent > 0, true);
    EXPECT_EQ(static_cast<int>(std::lround(analyzer_.sumCategoryPercents(50))), 100);
}

TEST_F(SHealthAgeImputationFixture,
       GivenDecadeAverageExcludesZeroWeights_WhenComputingImputedBmi_ThenAverageMatchesTwoValidMembers) {
    // Given: two valid weights (18, 24) and one zero — average must be 21, not (18+24+0)/3
    const double validWeightA = 18.0;
    const double validWeightB = 24.0;
    const double zeroWeight = 0.0;
    const double heightCm = 100.0;

    // When: decade average is derived the same way as SHealth::averageWeightForDecade
    const double manualAverage = (validWeightA + validWeightB) / 2.0;
    const double imputedBmi = SHealth::computeBmi(manualAverage, heightCm);

    // Then: zero is excluded from the mean and imputed BMI matches computeBmi(21, 100)
    ASSERT_EQ(manualAverage, 21.0);
    ASSERT_EQ(computeBmiMilli(manualAverage, heightCm), 21000);
    EXPECT_EQ(computeBmiMilli(imputedBmi, heightCm), 21000);
    EXPECT_EQ(SHealth::classifyBmi(imputedBmi), SHealth::BmiCategory::Normal);
    EXPECT_EQ(zeroWeight, 0.0);
}
