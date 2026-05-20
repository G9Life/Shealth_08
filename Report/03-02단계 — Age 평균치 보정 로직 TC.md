# SHealth_08 — Age 평균치 보정 로직 단위테스트 보고서

| 항목 | 내용 |
|------|------|
| **프로젝트명** | SHealth BMI (C++) |
| **작성일** | 2026-05-20 |
| **단계** | Activities 3 — TDD / 단위테스트 (03-02) |
| **대상** | 연령대별 체중 0 보정, `belongsToAgeDecade`, `loadAndCalculate` 파이프라인 |
| **테스트 파일** | `src/test/cpp/SHealthBMITest.cpp` |
| **선행 문서** | [03-01단계 — BMI 계산 로직 TC.md](./03-01단계%20—%20BMI%20계산%20로직%20TC.md) · [02_리팩토링 통합보고서.md](./02_리팩토링%20통합보고서.md) |

---

## 1. 개요

### 1.1 작업 목적

`SHealth.cpp`의 **연령대(10년 단위) 평균 체중 보정** 로직에 대해 Google Test 기반 단위·통합 테스트를 설계·구현하였다. README·`.cursorrules` 도메인 규칙(체중 0 → 동일 연령대 유효 체중 평균 대체, 평균 계산 시 weight=0 제외)을 검증한다.

| 요구사항 | 충족 |
|----------|------|
| `TEST_F` 최소 5개 (Age 보정 로직 중심) | ✅ 6개 (신규 Fixture 2종) |
| `EXPECT_EQ` / `ASSERT_EQ`로 BMI 계산 검증 | ✅ milli-BMI·카테고리·비율 |
| `shealth.dat` 기반, **경계값 포함** | ✅ id=93730, 연령대 경계 나이 |
| Given-When-Then 주석 구조 | ✅ 전 `TEST_F` 적용 |
| `cmake --build build && ctest` Green | ✅ 15/15 통과 |

### 1.2 검증 대상 로직

구현 위치: `src/main/cpp/SHealth.cpp` (private 포함), 공개 API는 `SHealth.h`.

| 함수 / API | 접근 | 역할 |
|------------|------|------|
| `belongsToAgeDecade(age, ageDecade)` | public static | `ageDecade ≤ age < ageDecade + 10` |
| `averageWeightForDecade(ageDecade)` | private | 연령대 내 `weight ≠ 0` 레코드만 평균 |
| `imputeMissingWeightsByAgeDecade()` | private | 평균 > 0일 때만 `weight == 0`에 대체 |
| `calculateBmis()` | private | 보정 후 BMI 산출 |
| `loadAndCalculate(filename)` | public | 로드 → 보정 → BMI → 연령대별 분포 |

보정 알고리즘 (`loadAndCalculate` 파이프라인):

```
loadRecordsFromFile
  → imputeMissingWeightsByAgeDecade   // 연령대별 평균 체중 대체
  → calculateBmis
  → calculateDistributionsByAgeDecade
```

도메인 규칙 요약:

| 규칙 | 내용 |
|------|------|
| 연령대 | 20, 30, …, 70 (`kMinAgeDecade` ~ `kMaxAgeDecade`, span=10) |
| 평균 대상 | 동일 연령대에서 `weightKg != 0`인 행만 |
| 보정 대상 | 동일 연령대에서 `weightKg == 0`인 행 |
| 평균 불가 | 유효 체중이 없으면 `averageWeight == 0` → 보정 스킵 |
| BMI | 보정된 체중으로 `computeBmi` → `classifyBmi` |

---

## 2. 테스트 설계

### 2.1 Fixture 구성

| Fixture | 용도 | SetUp |
|---------|------|-------|
| `SHealthAgeDecadeFixture` | `belongsToAgeDecade` 연령 경계 | 없음 |
| `SHealthAgeImputationFixture` | 보정·파이프라인·BMI 검증 | `loadFixture(path)` → `loadAndCalculate(resolveDataFilePath(path))` |
| `SHealthBmiCalculationFixture` | (03-01) BMI 계산·분류 | 없음 |
| `SHealthLoadedDataFixture` | (03-01) `shealth.dat` 통합 | `loadAndCalculate("shealth.dat")` |

### 2.2 검증 전략

| 검증 대상 | 방법 |
|-----------|------|
| 보정 후 BMI | `computeBmiMilli(imputedWeight, height)` + `ASSERT_EQ` |
| BMI 분류 | `EXPECT_EQ(classifyBmi(...), BmiCategory::…)` |
| 연령대 소속 | `EXPECT_EQ(belongsToAgeDecade(...), true/false)` |
| 보정 반영(통합) | 소형 CSV fixture 로드 후 `getCategoryPercent` 정수 % `EXPECT_EQ` |
| 평균 제외 규칙 | 수동 평균 `(w1+w2)/2` 와 `computeBmi` 결과 일치 |

03-01과 동일하게 부동소수 BMI는 **milli-BMI** (`round(bmi × 1000)`) 정수 비교를 사용한다.

### 2.3 테스트 전용 픽스처 데이터

| 파일 | 목적 |
|------|------|
| `src/test/data/impute_three_members.csv` | 50대 3명, id=3 체중 0 → (18+24)/2=21 kg 보정 |
| `src/test/data/impute_all_zero_weights.csv` | 50대 전원 체중 0 → 보정 불가 |

---

## 3. 테스트 케이스 목록

### 3.1 `SHealthAgeDecadeFixture` — 연령대 구간 (1개)

| # | TEST_F 이름 | Given | When | Then |
|---|-------------|-------|------|------|
| 10 | `GivenAgeDecadeBoundaryAges_WhenBelongsToAgeDecade_ThenExpectedMembership` | age 19, 20, 29, 30, 57 | `belongsToAgeDecade` | 19∉20대, 20·29∈20대, 30∈30대, 57∈50대∉40대 |

### 3.2 `SHealthAgeImputationFixture` — 체중 보정·BMI (5개)

| # | TEST_F 이름 | Given | When | Then |
|---|-------------|-------|------|------|
| 11 | `GivenShealth93730ZeroWeightRow_WhenImputedWithDecade50Average_ThenBmiMilliAndCategoryMatch` | 50대 평균 83.398 kg, 키 167.6 cm | `computeBmi` + `classifyBmi` | milli=29690, `Obesity` |
| 12 | `GivenThreeMemberFixtureWithOneZeroWeight_WhenLoadAndCalculate_ThenPercentsReflectImputedBmis` | 3인 fixture (18/24/0 kg) | `loadAndCalculate` + 분포 조회 | BMI 18/24/21 → U/O/N 각 33%, 합 100% |
| 13 | `GivenDecadeWithOnlyZeroWeights_WhenLoadAndCalculate_ThenNoImputationAndAllUnderweight` | 전원 weight=0 | `loadAndCalculate` | BMI=0, 50대 100% 저체중 |
| 14 | `GivenLoadedShealthDat_WhenDecade50IncludesImputed93730_ThenObesityPercentIsPositive` | 전체 `shealth.dat` | `loadAndCalculate` + 50대 비만 % | imputed milli=29690, obesity%>0, 합 100% |
| 15 | `GivenDecadeAverageExcludesZeroWeights_WhenComputingImputedBmi_ThenAverageMatchesTwoValidMembers` | 18, 24, 0 kg | 수동 평균 (18+24)/2 | 평균=21, milli=21000, `Normal` |

> CTest 번호 10~15는 03-01 테스트(1~9)에 이어 discover 된 순서이다.

---

## 4. shealth.dat · 경계값 선정

### 4.1 체중 0 레코드

| 항목 | 값 |
|------|-----|
| **유일한 weight=0 행** | id=**93730** |
| age | 57 → **50대** (`50 ≤ age < 60`) |
| weight | 0 (결측) |
| height | 167.6 cm |

### 4.2 50대 평균 체중 (보정값)

Python으로 `shealth.dat` 전수 스캔 후 산출 (유효 레코드 검증 동일: `id≥0`, `age>0`, `height≥0`).

| 항목 | 값 |
|------|-----|
| 50대 인원 | 871명 |
| 유효 체중(`weight≠0`) | 870명 (93730 제외) |
| **평균 체중** | **83.39793103448275** kg |
| 보정 후 BMI | 29.690… |
| **milli-BMI** | **29690** |
| 분류 | **Obesity** (BMI ≥ 25.0) |

C++ 테스트 상수:

```cpp
constexpr double kShealth50DecadeAverageWeightKg = 83.39793103448275;
constexpr int kShealth93730ImputedBmiMilli = 29690;
```

### 4.3 연령대 경계 (belongsToAgeDecade)

README 규칙 `20 ≤ age < 30` (20대)과 동일하게 `SHealth::belongsToAgeDecade` 검증.

| age | 20대 | 30대 | 50대 | 비고 |
|-----|------|------|------|------|
| 19 | ✗ | — | — | 최소 연령대 미만 |
| 20 | ✓ | ✗ | — | 20대 하한 포함 |
| 29 | ✓ | ✗ | — | 20대 상한 포함 |
| 30 | ✗ | ✓ | — | 30대 하한 |
| 57 | ✗ | — | ✓ | id=93730 |

### 4.4 소형 fixture: impute_three_members.csv

보정 후 3가지 BMI 구간이 균등(33%씩)하도록 체중·키 설계 (height=100 cm로 정수 BMI).

| id | age | weight (kg) | 보정 후 | BMI | 분류 |
|----|-----|-------------|---------|-----|------|
| 1 | 55 | 18 | 18 | 18.0 | Underweight |
| 2 | 56 | 24 | 24 | 24.0 | Overweight |
| 3 | 57 | 0 | **21** (평균) | 21.0 | Normal |

---

## 5. Given-When-Then 예시

### 5.1 shealth.dat 보정 BMI (단위 수준)

```cpp
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
}
```

### 5.2 파이프라인 통합 (소형 CSV)

```cpp
TEST_F(SHealthAgeImputationFixture,
       GivenThreeMemberFixtureWithOneZeroWeight_WhenLoadAndCalculate_ThenPercentsReflectImputedBmis) {
    // Given: 3 members in the 50s; id=3 has weight=0 → imputed to (18+24)/2 = 21 kg at height 100 cm
    const size_t recordCount = loadFixture("src/test/data/impute_three_members.csv");
    ASSERT_EQ(recordCount, 3u);

    // When: decade-50 distribution is queried after imputation
    // ...

    // Then: BMIs 18 / 24 / 21 → Underweight / Overweight / Normal (one third each)
    EXPECT_EQ(underweightPercent, 33);
    EXPECT_EQ(normalPercent, 33);
    EXPECT_EQ(overweightPercent, 33);
}
```

---

## 6. 빌드·실행 결과

### 6.1 명령

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

### 6.2 결과 (2026-05-20)

```
100% tests passed, 0 tests failed out of 15
Total Test time (real) = 12.19 sec
```

| 구분 | 테스트 수 | 결과 |
|------|-----------|------|
| 03-01 `SHealthBmiCalculationFixture` | 6 | Passed |
| 03-01 `SHealthLoadedDataFixture` | 3 | Passed |
| **03-02 `SHealthAgeDecadeFixture`** | **1** | **Passed** |
| **03-02 `SHealthAgeImputationFixture`** | **5** | **Passed** |

CTest `WORKING_DIRECTORY`는 `${CMAKE_SOURCE_DIR}`이므로 `src/test/data/*.csv`와 `shealth.dat`를 프로젝트 루트 기준으로 읽는다.

---

## 7. 변경 파일 요약

| 파일 | 변경 |
|------|------|
| `src/test/cpp/SHealthBMITest.cpp` | 03-02 `TEST_F` 6개, 상수·Fixture 2종, G-W-T 주석 |
| `src/test/data/impute_three_members.csv` | 신규 — 보정 성공 3인 시나리오 |
| `src/test/data/impute_all_zero_weights.csv` | 신규 — 보정 불가 시나리오 |
| `src/main/cpp/SHealth.cpp` | 변경 없음 (기존 구현 검증만) |
| `CMakeLists.txt` | 변경 없음 |

---

## 8. 커버리지·한계

### 8.1 커버 범위

| 영역 | 커버 여부 |
|------|-----------|
| `belongsToAgeDecade` 경계 | ✅ |
| 평균 계산 시 weight=0 제외 | ✅ (수동 평균 + fixture) |
| `imputeMissingWeightsByAgeDecade` 성공 경로 | ✅ (3인 fixture + shealth 93730) |
| 평균 불가 시 보정 스킵 | ✅ (`impute_all_zero_weights.csv`) |
| 보정 후 `calculateBmis` / 분류 | ✅ milli-BMI·`BmiCategory` |
| `loadAndCalculate` E2E | ✅ shealth.dat + 소형 CSV |
| `averageWeightForDecade` 직접 호출 | ⚠️ private — 동작은 fixture·상수로 간접 검증 |
| 개별 `HealthRecord` 조회 | ⚠️ private — 분포·`computeBmi`로 검증 |

### 8.2 03-01 대비 보완

03-01 보고서 §8.1에서 “체중 0 보정 후 개별 BMI — 분포 합계로만 간접 검증”이었던 항목을, 03-02에서 **id=93730 기대 BMI(milli 29690)** 및 **전용 CSV fixture**로 명시적으로 검증한다.

### 8.3 향후 확장 제안

1. **테스트 접근자** (`SHEALTH_ENABLE_TEST_ACCESS`): `averageWeightForDecadeForTest`로 연령대별 평균 직접 assert
2. **다중 weight=0 행 fixture**: 한 연령대에 결측 2건 이상인 합성 데이터
3. **골든 파일**: 50대 분포 비율(예: obesity 75.20%)과 `getCategoryPercent` 스냅샷 비교
4. **Height=0 보정** (README Activities 4 예정) — 동일 패턴의 `TEST_F` 추가

---

## 9. 결론

- 연령대 **체중 0 평균 보정** 규칙을 `TEST_F` **6건**으로 검증하였다 (`belongsToAgeDecade` 1 + 보정·BMI 5).
- `shealth.dat`의 유일한 결측 행 **id=93730**에 대해 50대 평균 체중·보정 BMI(**milli 29690**, **Obesity**)를 `ASSERT_EQ`로 고정하였다.
- 소형 CSV fixture로 **보정 성공**(33/33/33% 분포)과 **보정 스킵**(100% 저체중) 경계를 분리 검증하였다.
- 03-01 포함 **전체 15개** 테스트가 **Green**이며, `loadAndCalculate` 파이프라인 회귀 검증에 사용할 수 있다.
