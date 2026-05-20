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
