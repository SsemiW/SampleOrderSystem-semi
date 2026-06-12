# 구현 계획


## 1. 목표 요약

역할(고객·주문 담당자·생산 담당자)별 워크플로우를 단일 프로세스에서 처리
데이터는 JSON 파일로 영속 보관하며, MVC 아키텍처로 계층을 분리

### 핵심 상태 흐름

```
RESERVED ──(승인, 재고 충분)──→ CONFIRMED ──→ RELEASE
RESERVED ──(승인, 재고 부족)──→ PRODUCING ──→ CONFIRMED ──→ RELEASE
RESERVED ──(거절)────────────→ REJECTED
```

---

## 2. PoC 재사용 계획

| PoC | 재사용 범위 | 수정/확장 사항 |
|-----|-----------|--------------|
| **ConsoleMVC** | MVC 골격 전체 (인터페이스 + 컨트롤러 + 뷰 구조) | 생산라인·모니터링·출고 컨트롤러·뷰 추가; 메모리 모델을 JSON 영속 모델로 교체 |
| **DataPersistence** | `JsonValue`, `JsonParser`, `JsonSerializer`, `JsonRepository<T>` | `Order` 모델 지원 추가; `IRepository<T>` 추상 인터페이스 도입 |
| **DataMonitor** | `ConsoleHelper` (Windows 콘솔 API), 박스 문자·CJK 너비 렌더링, 대시보드 레이아웃 | 독립 프로세스 → 메뉴 내 화면으로 통합; FileWatcher 제거(동기 조회로 대체) |
| **DummyDataGenerator** | 프로젝트 외부 독립 유틸로 유지 | 초기 테스트 데이터 생성 도구로만 사용 |

---

## 3. 디렉토리 구조

```
SampleOrderSystem/
├── Core/                        # 공통 유틸리티 (외부 라이브러리 없음)
│   ├── JsonValue.h              # variant 기반 JSON 타입
│   ├── JsonParser.h / .cpp      # JSON 문자열 → JsonValue
│   ├── JsonSerializer.h / .cpp  # JsonValue → JSON 문자열 (pretty print)
│   └── ConsoleHelper.h          # Windows 콘솔 API 래퍼
│
├── Repository/
│   ├── IRepository.h            # 제네릭 CRUD 인터페이스 (신규)
│   └── JsonRepository.h         # IRepository<T> JSON 파일 구현체
│
├── Model/                       # 도메인 구조체 + 비즈니스 모델 인터페이스·구현
│   ├── Sample.h                 # 시료 구조체 (id, name, avgProdTime, yield, stock)
│   ├── Order.h                  # 주문 구조체 + OrderStatus enum + fromJson/toJson
│   ├── ProductionJob.h          # 생산 작업 구조체 (orderId, 실생산량, 시작시각 등)
│   ├── ISampleModel.h           # 시료 모델 인터페이스
│   ├── IOrderModel.h            # 주문 모델 인터페이스
│   ├── IProductionModel.h       # 생산라인 모델 인터페이스 (신규)
│   ├── SampleModel.h / .cpp     # JSON 영속 구현 (JsonRepository<Sample> 사용)
│   ├── OrderModel.h / .cpp      # JSON 영속 구현 (JsonRepository<Order> 사용)
│   └── ProductionModel.h / .cpp # FIFO 큐 + 진행도 계산 + 완료 처리 (신규)
│
├── View/                        # 콘솔 I/O 인터페이스 + 구현
│   ├── IMenuView.h / ISampleView.h / IOrderView.h   # (ConsoleMVC 원본)
│   ├── IMonitorView.h / IProductionView.h / IShipmentView.h  # (신규)
│   ├── MenuView.h / .cpp        # 메인 메뉴 + 시스템 요약
│   ├── SampleView.h / .cpp
│   ├── OrderView.h / .cpp
│   ├── MonitorView.h / .cpp     # 대시보드 렌더링 (DataMonitor 기반)
│   ├── ProductionView.h / .cpp  # 생산라인 현황·큐 출력
│   └── ShipmentView.h / .cpp   # 출고 처리 화면
│
├── Controller/
│   ├── AppController.h / .cpp   # 메인 루프 + 메뉴 라우팅
│   ├── SampleController.h / .cpp
│   ├── OrderController.h / .cpp # 접수·승인·거절 + 생산라인 연동
│   ├── MonitorController.h / .cpp
│   ├── ProductionController.h / .cpp
│   └── ShipmentController.h / .cpp
│
├── data/                        # 런타임 JSON 저장소
│   ├── samples.json
│   └── orders.json
│
├── Test/                        # Google Test + Google Mock
│   ├── Mocks.h                  # Mock 클래스 모음 (각 Phase 테스트 공용)
│   ├── CoreTest.cpp             # Phase 0: JsonParser / JsonSerializer
│   ├── RepositoryTest.cpp       # Phase 1: 구조체 round-trip / JsonRepository CRUD
│   ├── SampleTest.cpp           # Phase 2: SampleModel + SampleController
│   ├── OrderTest.cpp            # Phase 3: OrderModel + OrderController
│   ├── MonitorTest.cpp          # Phase 4: MonitorController 재고 상태 판정
│   ├── ProductionTest.cpp       # Phase 5: ProductionModel + ProductionController
│   ├── ShipmentTest.cpp         # Phase 6: ShipmentController
│   ├── AppControllerTest.cpp    # Phase 7: AppController 라우팅 + buildSummary
│   └── IntegrationTest.cpp      # Phase 8: end-to-end 시나리오 (실제 파일 사용)
│
└── main.cpp                     # 의존성 주입 + 초기화
```

---

## 4. 데이터 스키마

### samples.json

```json
[
  {
    "id": 1,
    "name": "알파-GaN",
    "avgProductionTime": 30.0,
    "yield": 0.85,
    "stock": 50
  }
]
```

### orders.json

```json
[
  {
    "id": 1,
    "sampleId": 1,
    "customerName": "삼성전자",
    "quantity": 30,
    "status": "RESERVED",
    "createdAt": "2026-06-12 09:00:00"
  }
]
```

### production_jobs.json

```json
[
  {
    "orderId": 1,
    "sampleId": 1,
    "requiredAmount": 12,
    "avgProdTimeMin": 30.0,
    "totalProdTimeMin": 360.0,
    "startedAt": "2026-06-12 10:30:00",
    "queuePosition": 0
  }
]
```

> `queuePosition == 0` → 현재 생산 중 (started), `>= 1` → 대기 중  
> `startedAt`은 큐 선두 진입 시점에 기록; 경과 시간으로 현재 생산량 계산

---

## 5. 핵심 계산 공식

| 항목 | 공식 |
|------|------|
| 부족분 | `주문수량 - 현재재고` |
| 실생산량 | `⌈부족분 / (수율 × 0.9)⌉` |
| 총생산시간 (분) | `avgProductionTime × 실생산량` |
| 현재 생산량 | `min(⌊경과시간(분) / avgProductionTime⌋, 실생산량)` |
| 생산 완료 판단 | `경과시간(분) ≥ 총생산시간` |
| 재고 완충 후 CONFIRMED | 완료 시 `stock += 실생산량`, 주문 상태 `PRODUCING → CONFIRMED` |

---
