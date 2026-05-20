#include <gtest/gtest.h>

#include "SHealth.h"

namespace {

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

TEST(SHealthBMITest, ClassifyBmiCategories) {
    EXPECT_EQ(SHealth::classifyBmi(18.5), SHealth::BmiCategory::Underweight);
    EXPECT_EQ(SHealth::classifyBmi(18.6), SHealth::BmiCategory::Normal);
    EXPECT_EQ(SHealth::classifyBmi(22.9), SHealth::BmiCategory::Normal);
    EXPECT_EQ(SHealth::classifyBmi(23.0), SHealth::BmiCategory::Overweight);
    EXPECT_EQ(SHealth::classifyBmi(24.9), SHealth::BmiCategory::Overweight);
    EXPECT_EQ(SHealth::classifyBmi(25.0), SHealth::BmiCategory::Obesity);
}

TEST(SHealthBMITest, ComputesBmiFromWeightAndHeight) {
    EXPECT_NEAR(SHealth::computeBmi(79.5, 158.3), 31.725, 0.001);
    EXPECT_DOUBLE_EQ(SHealth::computeBmi(70.0, 0.0), 0.0);
}

TEST(SHealthBMITest, ValidatesRecordConstraints) {
    EXPECT_FALSE(SHealth::isValidRecord(-1, 30, 170.0));
    EXPECT_FALSE(SHealth::isValidRecord(1, 0, 170.0));
    EXPECT_FALSE(SHealth::isValidRecord(1, 30, -1.0));

    EXPECT_TRUE(SHealth::isValidRecord(0, 1, 0.0));
    EXPECT_TRUE(SHealth::isValidRecord(1, 66, 170.0));
}

TEST(SHealthBMITest, BelongsToAgeDecade) {
    EXPECT_TRUE(SHealth::belongsToAgeDecade(25, 20));
    EXPECT_TRUE(SHealth::belongsToAgeDecade(29, 20));
    EXPECT_FALSE(SHealth::belongsToAgeDecade(30, 20));
    EXPECT_FALSE(SHealth::belongsToAgeDecade(19, 20));
    EXPECT_TRUE(SHealth::belongsToAgeDecade(75, 70));
}

TEST_F(SHealthLoadedDataFixture, LoadSampleDataFile) {
    const double obesityPercent =
        analyzer_.getCategoryPercent(20, SHealth::BmiCategory::Obesity);
    EXPECT_GT(obesityPercent, 0.0);
}

TEST_F(SHealthLoadedDataFixture, AgeDecadeDistributionSumsToOneHundred) {
    SHealth::forEachAgeDecade([this](int ageDecade) {
        const double total = analyzer_.sumCategoryPercents(ageDecade);
        if (total > 0.0) {
            EXPECT_NEAR(total, 100.0, 0.01) << "age decade " << ageDecade;
        }
    });
}

TEST_F(SHealthLoadedDataFixture, GetCategoryPercentAcceptsLegacyCategoryCode) {
    EXPECT_EQ(analyzer_.getCategoryPercent(20, SHealth::BmiCategory::Normal),
              analyzer_.getCategoryPercent(20, 200));
    EXPECT_EQ(analyzer_.getBmiRatio(20, 200),
              analyzer_.getCategoryPercent(20, SHealth::BmiCategory::Normal));
}
