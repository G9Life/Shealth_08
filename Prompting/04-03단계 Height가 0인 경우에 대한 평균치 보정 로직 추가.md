# Height adjustment in BMI calculation
_Exported on 5/20/2026 at 11:51:22 GMT+9 from Cursor (3.4.17)_

---

**User**

@bmi.png
@shealth.dat
@SHealth.cpp @SHealth.h @SHealthBMI.cpp 

[P] Persona
너는 C++17 기반 건강 데이터 분석 프로그램을 개선하는 시니어 C++ 개발자다.
기존 테스트와 public API를 유지하면서, 결측 데이터 보정 로직을 안전하게 확장한다.

[C] Context
프로젝트는 C:\DEV\Shealth_08의 SHealth BMI 분석 프로그램이다.
현재 weightKg == 0인 경우에는 같은 연령대의 평균 체중으로 보정하는 로직이 있다.
하지만 heightCm == 0인 경우에는 BMI 계산 결과가 0.0이 되어 저체중으로 분류될 수 있다.

현재 주요 처리 흐름은 다음과 같다.

loadAndCalculate()
  -> loadRecordsFromFile()
  -> imputeMissingWeightsByAgeDecade()
  -> calculateBmis()
  -> calculateDistributionsByAgeDecade()
이번 개선에서는 heightCm == 0도 결측값으로 보고, 같은 연령대의 평균 키로 보정해야 한다.

[T] Task
Height가 0인 레코드에 대해 같은 연령대의 평균 키를 계산해 보정하는 로직을 추가해줘.

구현 조건:

heightCm == 0.0은 결측 키로 판단한다.
같은 연령대에서 heightCm > 0.0인 레코드만 평균 키 계산에 사용한다.
평균 키를 계산할 수 없는 연령대는 보정하지 않는다.
BMI 계산 전에 키 보정이 완료되어야 한다.
기존 weightKg == 0 보정 로직은 유지한다.
heightCm < 0.0인 레코드는 기존처럼 invalid record로 제외한다.
기존 테스트는 모두 통과해야 한다.
신규 Google Test를 추가한다.
가능하면 체중 보정과 키 보정의 중복을 줄여 SRP 리팩토링 여지도 남긴다.
추천 처리 흐름:

loadAndCalculate()
  -> loadRecordsFromFile()
  -> imputeMissingWeightsByAgeDecade()
  -> imputeMissingHeightsByAgeDecade()
  -> calculateBmis()
  -> calculateDistributionsByAgeDecade()

[F] Format
결과는 아래 형식으로 정리해줘.

변경 요약
보정 정책
수정/추가된 메서드
추가한 테스트 케이스
테스트 실행 결과
남은 개선 후보
추천 테스트 케이스:
GivenThreeMemberFixtureWithOneZeroHeight_WhenLoadAndCalculate_ThenBmiUsesDecadeAverageHeight

---

**Cursor**

키 보정 로직을 추가하기 위해 기존 체중 보정 구현과 관련 파일을 먼저 살펴보겠습니다.


연령대별 평균 보정 공통 로직을 추출하고 `HeightImputer`를 추가합니다.


BMI 경계값을 재계산해 테스트 픽스처 수치를 맞춥니다.


## 변경 요약

`heightCm == 0.0`인 레코드를 같은 연령대의 평균 키로 보정하는 로직을 추가했습니다. 체중·키 보정의 공통 패턴은 `AgeDecadeImputer`로 추출했고, `SHealth::loadAndCalculate()` 파이프라인에 키 보정 단계를 체중 보정 직후·BMI 계산 전에 삽입했습니다.

## 보정 정책

| 항목 | 규칙 |
|------|------|
| 결측 판정 | `heightCm == 0.0` |
| 평균 산출 | 같은 연령대에서 `heightCm > 0.0`인 레코드만 사용 |
| 보정 불가 | 해당 연령대에 유효 키가 없으면 `heightCm` 유지 |
| 실행 순서 | 로드 → 체중 보정 → **키 보정** → BMI 계산 → 분포 집계 |
| 제외 | `heightCm < 0.0`은 로드 시 invalid record로 제외 (기존과 동일) |

## 수정/추가된 메서드

| 파일 | 내용 |
|------|------|
| `AgeDecadeImputer` (신규) | `averageForDecade()`, `imputeMissingByAgeDecade()` — 연령대별 평균·보정 공통 로직 |
| `HeightImputer` (신규) | `imputeMissingHeights()` |
| `WeightImputer` | `AgeDecadeImputer` 위임으로 리팩터링 |
| `SHealth::loadAndCalculate()` | `HeightImputer::imputeMissingHeights()` 호출 추가 |

```28:37:c:\DEV\Shealth_08\src\main\cpp\SHealth.cpp
size_t SHealth::loadAndCalculate(const std::string& filename) {
    if (!HealthRecordCsvReader::loadFromFile(filename, records_)) {
        return 0;
    }

    WeightImputer::imputeMissingWeights(records_);
    HeightImputer::imputeMissingHeights(records_);
    BmiDistributionAnalyzer::calculateBmis(records_);
    distributionsByDecade_ = BmiDistributionAnalyzer::computeDistributionsByAgeDecade(records_);
```

## 추가한 테스트 케이스

| 테스트 | 검증 내용 |
|--------|-----------|
| `GivenThreeMemberFixtureWithOneZeroHeight_WhenLoadAndCalculate_ThenBmiUsesDecadeAverageHeight` | 3명 중 `height=0` → 평균 102.5cm 보정, 저체중/정상/과체중 각 33% |
| `GivenDecadeWithOnlyZeroHeights_WhenLoadAndCalculate_ThenNoImputationAndAllUnderweight` | 전원 `height=0` → 보정 없음, BMI 0 → 100% 저체중 |
| `GivenDecadeAverageExcludesZeroHeights_WhenComputingImputedBmi_ThenAverageMatchesTwoValidMembers` | `height=0`은 평균 산출에서 제외 |

픽스처: `src/test/data/impute_three_members_zero_height.csv`, `impute_all_zero_heights.csv`

## 테스트 실행 결과

```
27/27 tests passed (100%)
Total Test time (real) = 17.73 sec
```

기존 24개 + 신규 3개 모두 통과했습니다.

## 남은 개선 후보

1. **`DataImputer` 통합** — `WeightImputer` / `HeightImputer`를 단일 파사드로 묶어 `loadAndCalculate()` 호출 단순화
2. **`shealth.dat` 실데이터 TC** — `height=0` 행이 추가되면 id 기반 통합 테스트 보강
3. **체중·키 동시 결측** — 동시에 0인 경우 보정 순서/상호 영향에 대한 명시적 TC
4. **`averageValue == 0.0` 스킵 조건** — 유효 평균이 실제 0인 극단 케이스와 “보정 불가” 구분 (현재 체중·키와 동일한 휴리스틱)

---

**User**

Report 폴더에 신규 파일 "04-03단계 Height가 0인 경우에 대한 평균치 보정 로직 추가
" 보고서를 작성해줘

---

**Cursor**

기존 Report 문서 형식을 확인한 뒤, 동일한 스타일로 Height 보정 보고서를 작성합니다.


기존 Report 형식에 맞춰 04-03 보고서를 작성하고 README 목록을 갱신합니다.


`Report/04-03단계 — Height가 0인 경우에 대한 평균치 보정 로직 추가.md` 보고서를 작성했고, `Report/README.md` 목록에도 반영했습니다.

보고서 구성은 다음과 같습니다.

| 섹션 | 내용 |
|------|------|
| **1. 개요** | 작업 목적, Before/After 파이프라인, 도메인 규칙 |
| **2. 구현 설계** | `AgeDecadeImputer` / `HeightImputer` / `WeightImputer` SRP·DRY 구조 |
| **3~4. 테스트** | Fixture, 검증 전략, `TEST_F` 3건 목록 |
| **5. 경계값** | `impute_three_members_zero_height.csv` 수치, `shealth.dat` 영향 |
| **6. G-W-T 예시** | 권장 TC 코드 스니펫 |
| **7. 빌드 결과** | **27/27** Green |
| **8. 변경 파일** | 신규·수정 파일 표 |
| **9~10. 한계·결론** | 미커버 항목, 향후 확장 제안 |

기존 `03-02`(체중 보정 TC), `04-02`(기능 추가) 보고서와 같은 표·mermaid·G-W-T 형식을 맞춰 두었습니다.

