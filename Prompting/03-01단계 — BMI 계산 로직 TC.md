# BMI calculation logic unit tests
_Exported on 5/20/2026 at 09:59:11 GMT+9 from Cursor (3.4.17)_

---

**User**

@shealth.dat
@SHealth.cpp @SHealth.h @SHealthBMI.cpp 

[P] 테스트 설계에 강한 시니어 C++ QA입니다.
[C] C++17, Google Test
[T] SHealthBMI.cpp 에서 BMI 계산 로직 TC전체 함수에 최소 5개 TEST_F를 작성해줘.
    - EXPECT_EQ/ASSERT_EQ로 BMI 계산 로직 검증
    - shealth.dat 를 참고해서 경계값 포함
    - Given-When-Then 주석 구조 사용
[F] 완성된 테스트 코드. cmake --build build && ctest가 Green이 되게 작성


---

**Cursor**

BMI 계산 로직과 `shealth.dat` 경계값을 파악한 뒤 Google Test `TEST_F`를 작성하겠습니다.


`shealth.dat` 샘플 레코드로 BMI·분류 경계값을 계산하고, `TEST_F` 5개 이상으로 테스트를 작성합니다.


`TEST_F` 6개 이상으로 BMI 계산·분류 테스트를 작성합니다.


`SHealthBMITest.cpp`에 BMI 계산·분류용 `TEST_F` 6개와 `shealth.dat` 로드 통합 `TEST_F` 3개를 추가했습니다. `cmake --build build` 및 `ctest` 모두 통과했습니다 (9/9 Green).

## 테스트 구성

### `SHealthBmiCalculationFixture` — BMI 계산·분류 (6개)

| TEST_F | shealth.dat 근거 | 검증 |
|--------|------------------|------|
| `GivenShealth93705ObesityRow_...` | id=93705 (79.5kg, 158.3cm) | `ASSERT_EQ` milli-BMI 31725, `EXPECT_EQ` Obesity |
| `GivenShealth93711NormalRow_...` | id=93711 (62.1kg, 170.6cm) | milli 21337, Normal |
| `GivenShealth93708OverweightRow_...` | id=93708 (53.5kg, 150.2cm) | milli 23714, Overweight |
| `GivenShealthUnderweightAndNearBoundaryRows_...` | id=93795, 97948 (≈18.5 경계) | Underweight |
| `GivenShealthNormalOverweightBoundaryRows_...` | id=94663, 94457 (23.0 전후) | Normal / Overweight |
| `GivenShealthObesityBoundaryAndExactThresholds_...` | id=102210 + 18.5/22.9/24.9/25.0, height=0 | 경계 `EXPECT_EQ`, zero-height → 0.0 |

### `SHealthLoadedDataFixture` — 파이프라인 통합 (3개)

- 로드 후 20대 obesity 비율 > 0
- 연령대별 카테고리 합계 100 (`EXPECT_EQ` 정수 반올림)
- 레거시 category code(200) ↔ enum API 일치

## 설계 포인트

- **Given-When-Then** 주석을 모든 `TEST_F`에 적용했습니다.
- 부동소수 BMI는 `shealth.dat`에서 역산한 **milli 단위 정수**(`ASSERT_EQ`)로 검증하고, 분류는 `BmiCategory` enum으로 `EXPECT_EQ` 검증합니다.
- 정확한 경계값(18.5, 22.9, 24.9, 25.0)은 height=100cm로 `computeBmi`가 정확히 나오도록 구성했습니다.
- BMI 임계값: Underweight ≤18.5, Normal <23.0, Overweight <25.0, Obesity ≥25.0 (`SHealth.cpp`와 동일)

```bash
cmake --build build
ctest --test-dir build --output-on-failure
# 100% tests passed, 0 tests failed out of 9
```

---

**User**

report 폴더에 보고서로 작성해줘

---

**Cursor**

Report 폴더의 기존 보고서 형식을 확인한 뒤 테스트 작업 보고서를 작성합니다.


기존 보고서 형식에 맞춰 테스트 보고서를 작성하고 README에 등록합니다.


`Report` 폴더에 테스트 보고서를 추가했습니다.

## 작성 문서

**[Report/03_BMI_계산_로직_단위테스트_보고서.md](c:\DEV\Shealth_08\Report\03_BMI_계산_로직_단위테스트_보고서.md)**

포함 내용:

1. **개요** — 요구사항 충족 여부, 검증 대상 API·분류 임계값  
2. **테스트 설계** — Fixture 2종, milli-BMI 검증 전략  
3. **테스트 케이스 목록** — `TEST_F` 9개 (Given-When-Then 표)  
4. **shealth.dat 샘플** — 사용 id 8건, 경계값 선정 근거·milli 표  
5. **G-W-T 코드 예시**  
6. **빌드·ctest 결과** — 9/9 Green  
7. **변경 파일·커버리지 한계·향후 제안**

`Report/README.md` 목록에도 Activities 3 항목으로 등록해 두었습니다.

