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

