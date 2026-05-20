# Age average correction logic test cases
_Exported on 5/20/2026 at 10:14:16 GMT+9 from Cursor (3.4.17)_

---

**User**

03-02단계 — Age 평균치 보정 로직 TC

@shealth.dat
@SHealth.cpp @SHealth.h @SHealthBMI.cpp 

[P] 테스트 설계에 강한 시니어 C++ QA입니다.
[C] C++17, Google Test
[T] SHealthBMI.cpp 에서 Age 평균치 보정 로직 TC를 전체 함수에 최소 5개 TEST_F를 작성해줘.
    - EXPECT_EQ/ASSERT_EQ로 BMI 계산 로직 검증
    - shealth.dat 를 참고해서 경계값 포함
    - Given-When-Then 주석 구조 사용
[F] 완성된 테스트 코드. cmake --build build && ctest가 Green이 되게 작성

---

**Cursor**

Age 평균치 보정 로직과 `shealth.dat` 경계값을 파악한 뒤, 기존 BMI 테스트 패턴에 맞춰 TEST_F를 작성합니다.


테스트 픽스처 CSV를 추가하고, Age 보정 로직용 TEST_F 5개 이상을 `SHealthBMITest.cpp`에 작성합니다.


Age 평균치 보정 로직용 **TEST_F 6개**를 `SHealthBMITest.cpp`에 추가했고, `cmake --build build && ctest` **15/15 Green**입니다.

## 추가된 테스트 (03-02)

| Fixture | 테스트 | 검증 대상 |
|---------|--------|-----------|
| `SHealthAgeDecadeFixture` | `GivenAgeDecadeBoundaryAges_...` | `belongsToAgeDecade` (19/20/29/30, id=93730→50대) |
| `SHealthAgeImputationFixture` | `GivenShealth93730ZeroWeightRow_...` | `shealth.dat` 50대 평균(83.398kg) → BMI milli **29690**, **Obesity** |
| | `GivenThreeMemberFixtureWithOneZeroWeight_...` | `loadAndCalculate` + 보정 후 33/33/33% 분포 |
| | `GivenDecadeWithOnlyZeroWeights_...` | 평균 불가 시 보정 스킵, BMI=0, 100% 저체중 |
| | `GivenLoadedShealthDat_WhenDecade50IncludesImputed93730_...` | 실데이터 통합 + 보정 BMI 검증 |
| | `GivenDecadeAverageExcludesZeroWeights_...` | 평균 계산 시 `weight=0` 제외 (18+24)/2=21 |

## shealth.dat 기준 상수

- **id=93730**: age=57, weight=0, height=167.6cm → 유일한 `weight=0` 행
- **50대 평균 체중**: 870명 유효 / 83.397931… kg
- **보정 후 BMI**: milli **29690** → `Obesity`

## 테스트 픽스처 CSV

- `src/test/data/impute_three_members.csv` — 18/24/0 kg → 보정 21 kg
- `src/test/data/impute_all_zero_weights.csv` — 보정 불가 케이스

모든 테스트는 **Given-When-Then** 주석과 `ASSERT_EQ`/`EXPECT_EQ`(milli-BMI·카테고리·비율)를 사용합니다.

---

**User**

Report 폴더에 보고서로 작성해줘

---

**Cursor**

03-01 보고서 형식을 참고해 03-02 Age 보정 TC 보고서를 작성합니다.


`Report/03-02단계 — Age 평균치 보정 로직 TC.md`에 보고서를 작성했습니다.

03-01 보고서와 같은 구성으로 정리했습니다.

1. **개요** — 요구사항 충족, 검증 대상 API·보정 파이프라인  
2. **테스트 설계** — Fixture 2종, milli-BMI 전략, 픽스처 CSV  
3. **테스트 케이스** — 신규 `TEST_F` 6개 (G-W-T 표)  
4. **shealth.dat** — id=93730, 50대 평균 83.398 kg, milli 29690, 연령 경계  
5. **G-W-T 예시** — 단위·통합 코드 스니펫  
6. **빌드 결과** — 15/15 Green  
7. **변경 파일** — 테스트·픽스처 CSV 목록  
8. **커버리지·한계** — private 함수 간접 검증, 향후 확장  
9. **결론**

선행 문서로 [03-01단계 — BMI 계산 로직 TC.md](Report/03-01단계%20—%20BMI%20계산%20로직%20TC.md)를 링크해 두었습니다.

