# 반도체 시료 생산주문관리 시스템

## 프로젝트 개요

반도체 시료의 주문·생산·출고를 관리하는 콘솔 기반 C++ 애플리케이션
역할(고객, 주문 담당자, 생산 담당자)별로 시료 등록, 주문 접수·승인·거절, 생산라인 운영, 출고 처리 기능을 제공

## 빌드 환경

- **IDE**: Visual Studio 2022 (PlatformToolset v145, VCProjectVersion 18.0)
- **언어 표준**: C++20 (`stdcpp20`)
- **플랫폼**: Win32 / x64
- **구성**: Debug / Release
- **빌드 도구**: MSBuild

### MSBuild 빌드 명령

```powershell
# Debug x64 빌드
msbuild SampleOrderSystem.vcxproj /p:Configuration=Debug /p:Platform=x64

# Release x64 빌드
msbuild SampleOrderSystem.vcxproj /p:Configuration=Release /p:Platform=x64

# NuGet 패키지 복원 후 빌드 (최초 또는 패키지 없을 때)
nuget restore SampleOrderSystem.vcxproj
msbuild SampleOrderSystem.vcxproj /p:Configuration=Debug /p:Platform=x64
```

## 테스트

- **프레임워크**: Google Test + Google Mock (gmock 1.11.0, NuGet 설치)
- 패키지 경로: `..\packages\gmock.1.11.0\build\native\`
- 빌드 전 `packages\gmock.1.11.0\build\native\gmock.targets` 존재 여부를 검사함 — 없으면 NuGet 복원 필요

```powershell
# NuGet 패키지 복원
nuget restore packages.config -PackagesDirectory ..\packages
```

## 도메인 모델

### 주문 상태 흐름

```
RESERVED → CONFIRMED (재고 충분)
RESERVED → PRODUCING (재고 부족, 생산라인 등록)
RESERVED → REJECTED  (거절)
PRODUCING → CONFIRMED (생산 완료)
CONFIRMED → RELEASE   (출고 처리)
```

| 상태 | 의미 |
|------|------|
| RESERVED | 주문 접수 |
| REJECTED | 주문 거절 (모니터링 제외) |
| PRODUCING | 승인 완료, 재고 부족으로 생산 중 |
| CONFIRMED | 승인 완료, 출고 대기 |
| RELEASE | 출고 완료 |

### 핵심 개념

- **시료(Sample)**: 고유 ID·이름·평균생산시간·수율 보유. 등록된 시료만 주문 가능
- **수율**: `정상 시료 / 총 생산 시료`
- **실생산량**: `ceil(부족분 / (수율 × 0.9))`
- **총생산시간**: `평균생산시간 × 실생산량`
- **생산 큐 스케줄링**: FIFO (선입선출)

### 재고 상태 표기 (모니터링)

| 표기 | 조건 |
|------|------|
| 여유 | 주문 대비 재고 충분 |
| 부족 | 주문 대비 재고 수량 부족 |
| 고갈 | 수량 0 |

## 주요 기능 모듈

| 모듈 | 책임 |
|------|------|
| 시료 관리 | 시료 등록·조회·검색 |
| 주문 | 주문 접수(RESERVED 생성) |
| 주문 승인/거절 | RESERVED → CONFIRMED/PRODUCING/REJECTED 전환 |
| 모니터링 | 상태별 주문 수, 시료별 재고 현황 |
| 생산라인 | 생산 현황 및 대기 큐(FIFO) 표시, 생산 완료 시 CONFIRMED 전환 |
| 출고 처리 | CONFIRMED → RELEASE 전환 |
