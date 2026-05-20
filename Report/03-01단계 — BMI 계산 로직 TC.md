# SHealth_08 — BMI 계산 로직 단위테스트 보고서

| 항목 | 내용 |
|------|------|
| **프로젝트명** | SHealth BMI (C++) |
| **작성일** | 2026-05-20 |
| **단계** | Activities 3 — TDD / 단위테스트 |
| **대상** | `SHealth::computeBmi`, `SHealth::classifyBmi`, `loadAndCalculate` 파이프라인 |
| **테스트 파일** | `src/test/cpp/SHealthBMITest.cpp` |
| **선행 문서** | [01_SHealth_08_프로젝트_분석_보고서.md](./01_SHealth_08_프로젝트_분석_보고서.md) · [02_리팩토링 통합보고서.md](./02_리팩토링%20통합보고서.md) |

---

## 1. 개요

### 1.1 작업 목적

`SHealthBMI` 실행 파일이 사용하는 **BMI 계산·분류 로직**(`SHealth.cpp`)에 대해 Google Test 기반 단위테스트를 설계·구현하였다. 요구사항은 다음과 같다.

| 요구사항 | 충족 |
|----------|------|
| `TEST_F` 최소 5개 (BMI 계산 로직 중심) | ✅ 6개 (`SHealthBmiCalculationFixture`) |
| `EXPECT_EQ` / `ASSERT_EQ`로 검증 | ✅ |
| `shealth.dat` 기반, **경계값 포함** | ✅ |
| Given-When-Then 주석 구조 | ✅ 전 `TEST_F` 적용 |
| `cmake --build build && ctest` Green | ✅ 9/9 통과 |

### 1.2 검증 대상 로직

BMI 관련 공개 API (`SHealth.h`):

| API | 역할 |
|-----|------|
| `computeBmi(weightKg, heightCm)` | BMI = 체중(kg) ÷ (키(m))², 키 0이면 0.0 반환 |
| `classifyBmi(bmi)` | BMI 구간 → `BmiCategory` enum |
| `loadAndCalculate(filename)` | CSV 로드 → 체중 보정 → BMI 계산 → 연령대별 분포 |

분류 임계값 (`SHealth.cpp` private 상수):

| 구간 | 조건 | `BmiCategory` |
|------|------|---------------|
| 저체중 | BMI ≤ 18.5 | `Underweight` (100) |
| 정상 | 18.5 < BMI < 23.0 | `Normal` (200) |
| 과체중 | 23.0 ≤ BMI < 25.0 | `Overweight` (300) |
| 비만 | BMI ≥ 25.0 | `Obesity` (400) |

---

## 2. 테스트 설계

### 2.1 Fixture 구성

| Fixture | 용도 | SetUp |
|---------|------|-------|
| `SHealthBmiCalculationFixture` | 단위 레벨: `computeBmi` / `classifyBmi` | 없음 (정적 API만 사용) |
| `SHealthLoadedDataFixture` | 통합: 실제 `shealth.dat` 로드 후 분포·API 검증 | `loadAndCalculate(resolveDataFilePath("shealth.dat"))` |

### 2.2 검증 전략

#### 부동소수 BMI 값

`shealth.dat` 실측값(예: 79.5kg, 158.3cm)은 IEEE 754 표현 오차로 `EXPECT_DOUBLE_EQ`와 단순 `EXPECT_EQ(double)`가 불안정할 수 있다. 따라서:

- **milli-BMI**: `round(bmi × 1000)`을 정수로 변환 후 `ASSERT_EQ` 비교
- **카테고리**: `EXPECT_EQ(SHealth::classifyBmi(...), BmiCategory::…)` 로 enum 비교
- **정확 표현 가능한 경계**: height = 100cm일 때 `computeBmi(18.5, 100.0) == 18.5` 등은 `EXPECT_EQ(double)` 직접 비교

#### 헬퍼 (익명 네임스페이스)

```cpp
int bmiToMilli(double bmi);
int computeBmiMilli(double weightKg, double heightCm);
```

---

## 3. 테스트 케이스 목록

### 3.1 `SHealthBmiCalculationFixture` — BMI 계산·분류 (6개)

| # | TEST_F 이름 | Given | When | Then |
|---|-------------|-------|------|------|
| 1 | `GivenShealth93705ObesityRow_WhenComputeBmi_ThenMilliMatchesExpected` | id=93705 (79.5kg, 158.3cm) | `computeBmi` | milli=31725, `Obesity` |
| 2 | `GivenShealth93711NormalRow_WhenComputeBmi_ThenMilliAndCategoryMatch` | id=93711 (62.1kg, 170.6cm) | `computeBmi` + `classifyBmi` | milli=21337, `Normal` |
| 3 | `GivenShealth93708OverweightRow_WhenComputeBmi_ThenMilliAndCategoryMatch` | id=93708 (53.5kg, 150.2cm) | `computeBmi` + `classifyBmi` | milli=23714, `Overweight` |
| 4 | `GivenShealthUnderweightAndNearBoundaryRows_WhenClassify_ThenExpectedCategories` | id=93795, 97948 | `classifyBmi` | 둘 다 `Underweight` |
| 5 | `GivenShealthNormalOverweightBoundaryRows_WhenClassify_ThenExpectedCategories` | id=94663, 94457 | `classifyBmi` | `Normal` / `Overweight` (23.0 전후) |
| 6 | `GivenShealthObesityBoundaryAndExactThresholds_WhenComputeAndClassify_ThenExpected` | id=102210 + 이론 경계 | `computeBmi` + `classifyBmi` | 24.9→Overweight, 25.0→Obesity, height=0→0.0 |

### 3.2 `SHealthLoadedDataFixture` — 데이터 파이프라인 (3개)

| # | TEST_F 이름 | 검증 내용 |
|---|-------------|-----------|
| 7 | `GivenLoadedShealthDat_WhenQueryAgeDecade20_ThenObesityPercentIsPositive` | 로드 후 20대 비만 비율 > 0 |
| 8 | `GivenLoadedShealthDat_WhenSummingCategoryPercents_ThenEachDecadeTotalsOneHundred` | 비어 있지 않은 연령대 합계 = 100% |
| 9 | `GivenLoadedShealthDat_WhenUsingLegacyCategoryCode_ThenPercentMatchesEnumApi` | enum(200) ↔ `getBmiRatio` 일치 |

---

## 4. shealth.dat 샘플·경계값 선정

### 4.1 데이터셋 개요

| 항목 | 값 |
|------|-----|
| 파일 | `shealth.dat` (프로젝트 루트) |
| 형식 | CSV: `id,age,weight,height` |
| 유효 레코드 수 | 4,821건 (헤더 제외, 검증 통과 건) |
| BMI 분포 (대략) | 저체중 78 · 정상 654 · 과체중 501 · 비만 3,589 |

### 4.2 테스트에 사용한 대표 행

| id | weight (kg) | height (cm) | BMI (계산) | milli | 분류 | 선정 이유 |
|----|-------------|-------------|------------|-------|------|-----------|
| 93705 | 79.5 | 158.3 | 31.725 | 31725 | 비만 | 데이터셋 첫 행, 고BMI 대표 |
| 93711 | 62.1 | 170.6 | 21.337 | 21337 | 정상 | 정상 구간 대표 |
| 93708 | 53.5 | 150.2 | 23.714 | 23714 | 과체중 | 과체중 구간 대표 |
| 93795 | 44.5 | 168.6 | 15.655 | 15655 | 저체중 | weight>0 저체중 최초 샘플 |
| 97948 | 55.3 | 172.9 | 18.498 | 18498 | 저체중 | **18.5 경계** 근접 (≤18.5) |
| 94663 | 59.0 | 160.2 | 22.989 | 22989 | 정상 | **23.0 경계** 직전 (<23) |
| 94457 | 67.7 | 164.6 | 24.988 | 24988 | 과체중 | **25.0 경계** 직전 (<25) |
| 102210 | 74.3 | 172.4 | 24.999 | 24999 | 과체중 | 비만 직전 (≥25 미만) |

> id=93730 (weight=0)은 체중 보정 파이프라인 대상이나, 개별 BMI는 private 상태라 단위 `TEST_F`에서는 미검증. 통합 Fixture에서 분포 합계 100% 등으로 간접 검증.

### 4.3 이론 경계값 (height = 100 cm)

| 체중 (kg) | BMI | 기대 분류 |
|-----------|-----|-----------|
| 18.5 | 18.5 | Underweight (경계 포함) |
| 22.9 | 22.9 | Normal |
| 24.9 | 24.9 | Overweight |
| 25.0 | 25.0 | Obesity |
| 70.0, height 0 | 0.0 | Underweight (BMI≤18.5) |

### 4.4 경계값 탐색 방법

`shealth.dat` 전체를 스캔하여 임계값(18.5, 23.0, 25.0)에 **가장 근접한 유효 레코드**(weight>0, height>0)를 선정하였다. Python 스크립트로 milli 값을 사전 계산한 뒤 C++ 테스트 상수(`kShealth*`)에 반영하였다.

---

## 5. Given-When-Then 예시

```cpp
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
100% tests passed, 0 tests failed out of 9
Total Test time (real) = 8.31 sec
```

| # | 테스트 | 결과 |
|---|--------|------|
| 1–6 | `SHealthBmiCalculationFixture.*` | Passed |
| 7–9 | `SHealthLoadedDataFixture.*` | Passed |

CTest `WORKING_DIRECTORY`는 `${CMAKE_SOURCE_DIR}`로 설정되어 있어, `shealth.dat`를 프로젝트 루트에서 직접 읽는다 (`resolveDataFilePath` fallback: `../shealth.dat`).

---

## 7. 변경 파일 요약

| 파일 | 변경 |
|------|------|
| `src/test/cpp/SHealthBMITest.cpp` | `TEST_F` 9개, milli-BMI 헬퍼, shealth.dat 상수, G-W-T 주석 |
| `CMakeLists.txt` | 변경 없음 (기존 `gtest_discover_tests` 활용) |

---

## 8. 커버리지·한계

### 8.1 커버 범위

| 영역 | 커버 여부 |
|------|-----------|
| `computeBmi` 정상·0 키 | ✅ |
| `classifyBmi` 4구간·경계 | ✅ |
| `shealth.dat` 실데이터 행 | ✅ (8개 id + 이론 경계) |
| `loadAndCalculate` E2E | ✅ (Fixture 3건) |
| `isValidRecord`, `belongsToAgeDecade` | ⚠️ 본 단계에서 제외 (이전 TC 정리 시 삭제됨, 필요 시 별도 `TEST` 추가 가능) |
| 체중 0 보정 후 개별 BMI | ⚠️ private — 분포 합계로만 간접 검증 |

### 8.2 향후 확장 제안

1. **파라미터화 테스트** (`TEST_P`): id·weight·height·expectedCategory 테이블 드리븐
2. **체중 보정 전용 Fixture**: 보정 전후 BMI 변화를 테스트용 접근자 추가 시 검증 가능
3. **골든 파일 테스트**: `SHealthBMI` stdout과 연령대별 기대 비율 스냅샷 비교

---

## 9. 결론

- BMI **계산식**과 **4단계 분류**, **경계값(18.5 / 23.0 / 25.0)** 을 `shealth.dat` 실측 샘플과 이론값으로 `TEST_F` 6건 이상 검증하였다.
- 부동소수 오차는 **milli 정수 `ASSERT_EQ`** 로, 분류는 **`BmiCategory` `EXPECT_EQ`** 로 안정화하였다.
- 전체 9개 테스트가 **Green** 상태이며, 리팩토링 이후 회귀 검증에 사용할 수 있다.
