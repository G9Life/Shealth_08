# Creating .cursorrules file from analysis
_Exported on 5/19/2026 at 17:06:34 GMT+9 from Cursor (3.4.17)_

---

**User**

@README.md

위 파일과 SHEALTH_08 프로젝트를 분석해서 Root 폴더에 .cursorrules  파일을 만들어줘

---

**Cursor**

README와 프로젝트 구조를 먼저 살펴본 뒤, 그에 맞는 `.cursorrules`를 작성하겠습니다.


소스 코드와 CMake 설정을 확인해 프로젝트 규칙을 정확히 반영하겠습니다.


README·소스·CMake를 반영해 루트에 `.cursorrules` 파일을 작성합니다.


`README.md`, 소스 코드, `CMakeLists.txt`, TDD 프롬프트 파일을 분석해 프로젝트 루트에 `.cursorrules`를 생성했습니다.

## 포함된 내용

| 섹션 | 내용 |
|------|------|
| **프로젝트 목적** | BMI 통계 실습, 기존 코드 스멜·리팩토링 목표 |
| **빌드/실행** | CMake 3.10+, C++17, `ctest` 명령 |
| **디렉터리 구조** | `src/main/cpp`, `src/test/cpp` 유지 규칙 |
| **도메인 규칙** | BMI 공식, 4단계 분류, 연령대(10년 단위), weight=0 보정 |
| **리팩토링 가이드** | 현재 `SHealth`의 스멜 목록, SRP/DRY/enum·STL 권장 |
| **테스트** | Google Test TDD, 필수 TC 5가지 영역 |
| **AI 동작** | 한국어 응답, 단계적 리팩토링, git 자동 커밋 금지 |
| **Activities 4** | height=0 보정, 정상 BMI 목록, 전체 비율 등 예정 기능 |

현재 코드에서 README와 어긋날 수 있는 부분(예: 비만 분류가 `> 25`로 되어 있음, `type` 100/200 등 매직 넘버)도 규칙에 명시해 두었습니다.

> **참고**: 최신 Cursor는 `.cursor/rules/*.mdc` 형식도 지원합니다. 루트 `.cursorrules` 한 파일로 관리하는 방식을 요청하셨기 때문에 그 형식으로 작성했습니다. 파일별 규칙으로 나누고 싶으시면 말씀해 주세요.

---

**User**

Report 폴더에 보고서로 작성해줘

---

**Cursor**

Report 폴더에 프로젝트 분석 보고서를 작성합니다. 현재 프로젝트 상태를 확인합니다.


`Report` 폴더에 프로젝트 분석 보고서를 작성했습니다.

## 생성 파일

**`Report/SHealth_08_프로젝트_분석_보고서.md`**

## 보고서 구성

| 섹션 | 내용 |
|------|------|
| **1. 개요** | 배경, BMI·연령대·보정 요구사항 |
| **2. 기술 스택** | C++17, CMake, Google Test, 빌드/실행 명령 |
| **3. 코드 구조** | 파일 역할, 처리 흐름(mermaid), 주요 API |
| **4. 코드 스멜** | God Class, 매직 넘버, 24개 중복 변수, README vs 구현 불일치(BMI 25) |
| **5. 도메인 로직** | BMI·체중 보정·비율 계산 상세 |
| **6. 실습 로드맵** | Activities 6시간, 목표 클래스 다이어그램 |
| **7. 단위 테스트 계획** | 필수 TC 5영역, TDD 순서, 네이밍 예시 |
| **8. .cursorrules** | 수립 내용·활용 방법 |
| **9. 결론** | 현재 상태, 권장 다음 단계 |
| **부록** | 참고 파일, AI 프롬프트 예시 |

회고·발표(Activities 5)에 쓸 Before 분석과 리팩토링·테스트 계획을 한 문서에 모아 두었습니다. PDF나 영문 버전이 필요하면 알려 주세요.

