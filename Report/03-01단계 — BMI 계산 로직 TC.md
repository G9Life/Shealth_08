# SHealth_08 — BMI 계산 로직 단위테스트 보고서

| 항목 | 내용 |
|------|------|
| **프로젝트명** | SHealth BMI (C++) |
| **작성일** | 2026-05-20 |
| **단계** | Activities 3 — TDD / 단위테스트 (03-01) |
| **대상** | `SHealth::computeBmi`, `SHealth::classifyBmi`, `loadAndCalculate` 파이프라인 |
| **실행 진입점** | `src/main/cpp/SHealthBMI.cpp` → `SHealth` 라이브러리 (`SHealth.cpp`) |
| **테스트 파일** | `src/test/cpp/SHealthBMITest.cpp` |
| **참고 자료** | `bmi.png`, `shealth.dat` |
| **선행 문서** | [01_SHealth_08_프로젝트_분석_보고서.md](./01_SHealth_08_프로젝트_분석_보고서.md) · [02_리팩토링 통합보고서.md](./02_리팩토링%20통합보고서.md) |
| **후속 문서** | [03-03단계 — 정상/저체중/과체중/비만 분류 TC.md](./03-03단계%20—%20정상·저체중·과체중·비만%20분류%20TC.md) · [03-02단계 — Age 평균치 보정 로직 TC.md](./03-02단계%20—%20Age%20평균치%20보정%20로직%20TC.md) |

---

## 1. 개요

### 1.1 작업 목적

`SHealthBMI` 실행 파일이 사용하는 **BMI 계산·4단계 분류 로직**에 대해 Google Test 기반 단위·통합 테스트를 설계·구현하였다.

| 요구사항 | 충족 |
|----------|------|
| `TEST_F` 최소 5개 (BMI 계산·분류 중심) | ✅ 6개 (`SHealthBmiCalculationFixture`) |
| `EXPECT_EQ` / `ASSERT_EQ`로 BMI·분류 검증 | ✅ milli-BMI + `BmiCategory` |
| `shealth.dat` 기반, **경계값 포함** | ✅ 8개 id + 이론 임계값 |
| Given-When-Then 주석 구조 | ✅ 전 `TEST_F` 적용 |
| `cmake --build build && ctest` Green | ✅ 03-01 범위 9/9 통과 (전체 스위트 15/15) |

### 1.2 BMI 정의 및 분류 기준

![BMI](../bmi.png)

README·`bmi.png`와 구현(`SHealth.cpp`)의 분류 규칙:

| 구간 | README / 도메인 | 구현 조건 (`classifyBmi`) | `BmiCategory` 코드 |
|------|-----------------|---------------------------|-------------------|
| 저체중 | BMI ≤ 18.5 | `bmi <= 18.5` | `Underweight` (100) |
| 정상 | 18.5 < BMI < 23 | `bmi < 23.0` (상위 구간 제외 후) | `Normal` (200) |
| 과체중 | 23 ≤ BMI < 25 | `bmi < 25.0` | `Overweight` (300) |
| 비만 | BMI ≥ 25 | 그 외 | `Obesity` (400) |

계산식:

```
BMI = weight(kg) / (height(m))²
height(m) = height(cm) / 100
height(cm) == 0 → BMI = 0.0
```

### 1.3 검증 대상 API

| API | 접근 | 역할 |
|-----|------|------|
| `computeBmi(weightKg, heightCm)` | public static | BMI 산출 |
| `classifyBmi(bmi)` | public static | BMI → `BmiCategory` |
| `loadAndCalculate(filename)` | public | CSV 로드 → 체중 보정 → BMI → 연령대별 분포 |
| `getCategoryPercent(ageDecade, category)` | public | 연령대·구간별 비율(%) |
| `getBmiRatio` / legacy int code | public | 하위 호환 API |

> `SHealthBMI.cpp`는 `loadAndCalculate` 후 연령대별 분포를 출력하는 `main`만 담당한다. 본 보고서의 검증 대상 로직은 `SHealth.cpp`에 있다.

---

## 2. 테스트 설계

### 2.1 Fixture 구성

| Fixture | 용도 | SetUp |
|---------|------|-------|
| `SHealthBmiCalculationFixture` | 단위: `computeBmi` / `classifyBmi` | 없음 |
| `SHealthLoadedDataFixture` | 통합: 실제 `shealth.dat` 로드 후 분포·API | `loadAndCalculate(resolveDataFilePath("shealth.dat"))` |

### 2.2 검증 전략

#### 부동소수 BMI

`shealth.dat` 실측값(예: 79.5 kg, 158.3 cm)은 IEEE 754 오차로 `EXPECT_DOUBLE_EQ`가 불안정할 수 있다.

| 대상 | 방법 |
|------|------|
| BMI 수치 | **milli-BMI**: `round(bmi × 1000)` → `ASSERT_EQ` |
| 분류 | `EXPECT_EQ(classifyBmi(...), BmiCategory::…)` |
| 이론 경계 (height=100 cm) | `computeBmi(18.5, 100.0) == 18.5` 등 `EXPECT_EQ(double)` |

#### 헬퍼 (익명 네임스페이스)

```cpp
int bmiToMilli(double bmi);
int computeBmiMilli(double weightKg, double heightCm);
```

### 2.3 카테고리별 커버리지 매트릭스

| `BmiCategory` | 대표 TEST_F | shealth.dat id | 경계 |
|---------------|-------------|----------------|------|
| Obesity | #1 `GivenShealth93705…` | 93705 | 고BMI 대표 |
| Normal | #2 `GivenShealth93711…` | 93711 | 정상 구간 |
| Overweight | #3 `GivenShealth93708…` | 93708 | 과체중 구간 |
| Underweight | #4 `GivenShealthUnderweight…` | 93795, 97948 | **18.5** 이하 |
| Normal ↔ Overweight | #5 `GivenShealthNormalOverweight…` | 94663, 94457 | **23.0** 직전/이후 |
| 전 구간 + 이론 경계 | #6 `GivenShealthObesityBoundary…` | 102210 + 100 cm | **25.0**, 키 0 |

---

## 3. 테스트 케이스 목록

### 3.1 `SHealthBmiCalculationFixture` — BMI 계산·분류 (6개)

| # | TEST_F 이름 | Given | When | Then |
|---|-------------|-------|------|------|
| 1 | `GivenShealth93705ObesityRow_WhenComputeBmi_ThenMilliMatchesExpected` | id=93705 (79.5 kg, 158.3 cm) | `computeBmi` | milli=31725, `Obesity` |
| 2 | `GivenShealth93711NormalRow_WhenComputeBmi_ThenMilliAndCategoryMatch` | id=93711 (62.1 kg, 170.6 cm) | `computeBmi` + `classifyBmi` | milli=21337, `Normal` |
| 3 | `GivenShealth93708OverweightRow_WhenComputeBmi_ThenMilliAndCategoryMatch` | id=93708 (53.5 kg, 150.2 cm) | `computeBmi` + `classifyBmi` | milli=23714, `Overweight` |
| 4 | `GivenShealthUnderweightAndNearBoundaryRows_WhenClassify_ThenExpectedCategories` | id=93795, 97948 | `classifyBmi` | 둘 다 `Underweight` |
| 5 | `GivenShealthNormalOverweightBoundaryRows_WhenClassify_ThenExpectedCategories` | id=94663, 94457 | `classifyBmi` | `Normal` / `Overweight` |
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
| 유효 레코드 수 | 4,821건 (헤더 제외, `isValidRecord` 통과) |
| BMI 분포 (대략) | 저체중 78 · 정상 654 · 과체중 501 · 비만 3,589 |

### 4.2 테스트에 사용한 대표 행

| id | weight (kg) | height (cm) | BMI | milli | 분류 | 선정 이유 |
|----|-------------|-------------|-----|-------|------|-----------|
| 93705 | 79.5 | 158.3 | 31.725 | 31725 | 비만 | 데이터셋 첫 행, 고BMI |
| 93711 | 62.1 | 170.6 | 21.337 | 21337 | 정상 | 정상 구간 대표 |
| 93708 | 53.5 | 150.2 | 23.714 | 23714 | 과체중 | 과체중 구간 대표 |
| 93795 | 44.5 | 168.6 | 15.655 | 15655 | 저체중 | weight>0 저체중 최초 샘플 |
| 97948 | 55.3 | 172.9 | 18.498 | 18498 | 저체중 | **18.5 경계** 근접 (≤18.5) |
| 94663 | 59.0 | 160.2 | 22.989 | 22989 | 정상 | **23.0 경계** 직전 (<23) |
| 94457 | 67.7 | 164.6 | 24.988 | 24988 | 과체중 | **25.0 경계** 직전 (<25) |
| 102210 | 74.3 | 172.4 | 24.999 | 24999 | 과체중 | 비만 직전 (≥25 미만) |

### 4.3 이론 경계값 (height = 100 cm)

| 체중 (kg) | BMI | 기대 분류 |
|-----------|-----|-----------|
| 18.5 | 18.5 | Underweight (구현: ≤18.5) |
| 22.9 | 22.9 | Normal |
| 24.9 | 24.9 | Overweight |
| 25.0 | 25.0 | Obesity |
| 70.0, height 0 | 0.0 | Underweight (BMI≤18.5) |

### 4.4 경계값 탐색 방법

`shealth.dat` 전체를 스캔하여 임계값 **18.5 / 23.0 / 25.0**에 가장 근접한 유효 레코드(weight>0, height>0)를 선정하였다. Python으로 milli 값을 사전 계산한 뒤 C++ 상수(`kShealth*`)에 반영하였다.

```python
# milli 검증 예시
bmi = weight / ((height/100)**2)
milli = round(bmi * 1000)  # 93705 → 31725
```

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

4구간 경계 일괄 검증 (#6):

```cpp
// Given: exact thresholds at 100 cm height
EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(18.5, 100.0)), SHealth::BmiCategory::Underweight);
EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(22.9, 100.0)), SHealth::BmiCategory::Normal);
EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(24.9, 100.0)), SHealth::BmiCategory::Overweight);
EXPECT_EQ(SHealth::classifyBmi(SHealth::computeBmi(25.0, 100.0)), SHealth::BmiCategory::Obesity);
```

---

## 6. 빌드·실행 결과

### 6.1 명령

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows (PowerShell):

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

### 6.2 결과 (2026-05-20)

**03-01 범위 (BMI 계산·분류 9건)**

```
100% tests passed, 0 tests failed out of 9  (CTest #1–#9)
```

| # | Fixture | 결과 |
|---|---------|------|
| 1–6 | `SHealthBmiCalculationFixture.*` | Passed |
| 7–9 | `SHealthLoadedDataFixture.*` | Passed |

**전체 테스트 스위트 (03-02 Age 보정 포함 15건)**

```
100% tests passed, 0 tests failed out of 15
Total Test time (real) ≈ 7.3 sec
```

CTest `WORKING_DIRECTORY`는 `${CMAKE_SOURCE_DIR}` (`CMakeLists.txt`의 `gtest_discover_tests`)이므로 `shealth.dat`를 프로젝트 루트에서 직접 읽는다. `resolveDataFilePath`는 실패 시 `../shealth.dat` fallback을 시도한다.

---

## 7. 변경 파일 요약

| 파일 | 변경 |
|------|------|
| `src/test/cpp/SHealthBMITest.cpp` | 03-01 `TEST_F` 9개, milli-BMI 헬퍼, `kShealth*` 상수, G-W-T 주석 |
| `src/main/cpp/SHealth.cpp` | 변경 없음 (기존 구현 검증) |
| `src/main/cpp/SHealthBMI.cpp` | 변경 없음 |
| `CMakeLists.txt` | 변경 없음 (`gtest_discover_tests` 기존 설정) |

---

## 8. 커버리지·한계

### 8.1 커버 범위

| 영역 | 커버 여부 |
|------|-----------|
| `computeBmi` 정상·키 0 | ✅ |
| `classifyBmi` 4구간·경계 | ✅ |
| `shealth.dat` 실데이터 행 | ✅ (8 id + 이론 경계) |
| `loadAndCalculate` E2E | ✅ (LoadedData Fixture 3건) |
| 체중 0 보정 후 개별 BMI | ⚠️ 03-02에서 id=93730 등으로 보완 — [03-02 보고서](./03-02단계%20—%20Age%20평균치%20보정%20로직%20TC.md) |
| `belongsToAgeDecade` | ⚠️ 03-02에서 검증 |

### 8.2 향후 확장 제안

1. **파라미터화 테스트** (`TEST_P`): id·weight·height·expectedCategory 테이블 드리븐
2. **골든 파일 테스트**: `SHealthBMI` stdout과 연령대별 기대 비율 스냅샷 비교
3. **테스트 접근자**: private `HealthRecord` BMI 직접 assert (필요 시)

---

## 9. 결론

- BMI **계산식**과 **저체중/정상/과체중/비만** 4단계 분류를 `shealth.dat` 실측 8건과 이론 임계값으로 `TEST_F` **6건** 이상 검증하였다.
- 부동소수 오차는 **milli 정수 `ASSERT_EQ`**, 분류는 **`BmiCategory` `EXPECT_EQ`**로 안정화하였다.
- 통합 Fixture **3건**으로 `loadAndCalculate` 파이프라인·레거시 API를 검증하였다.
- 03-01 **9개** 테스트가 **Green**이며, 03-02 Age 보정 테스트와 합쳐 **전체 15개**가 회귀 검증에 사용된다.
