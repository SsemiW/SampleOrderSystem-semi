## 6. 단계별 구현 계획

---

### Phase 0 — 프로젝트 기반 설정

**목표**: 빌드 가능한 빈 골격 확보

#### 0-1. 공통 유틸리티 통합
- `DataPersistence`의 `JsonValue.h`, `JsonParser.h/.cpp`, `JsonSerializer.h/.cpp` → `Core/` 복사
- `DataMonitor`의 `ConsoleHelper.h` → `Core/` 복사
- 네임스페이스: `Core::` (충돌 방지)

#### 0-2. IRepository 인터페이스 정의
```cpp
// Repository/IRepository.h
template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual int64_t     create(T item)                   = 0;
    virtual std::optional<T> findById(int64_t id) const  = 0;
    virtual std::vector<T>   findAll() const             = 0;
    virtual void        update(const T& item)            = 0;
    virtual void        remove(int64_t id)               = 0;
};
```

#### 0-3. vcxproj 필터 갱신
- Core / Repository / Model / View / Controller / Test 필터 추가
- Google Mock NuGet 패키지 확인 (`packages\gmock.1.11.0\`)

#### 0-4. 단위 테스트 — `Test/CoreTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `JsonParser_Primitives` | null / bool / int / double / string 파싱 |
| `JsonParser_Array` | 배열 원소 접근, 타입 불일치 시 예외 |
| `JsonParser_Object` | 키 조회, 중첩 객체 파싱 |
| `JsonSerializer_RoundTrip` | `parse(serialize(v)) == v` (모든 타입) |
| `JsonSerializer_PrettyPrint` | 인덴트·줄바꿈 포함 출력 확인 |

**완료 조건**: `msbuild /p:Configuration=Debug /p:Platform=x64` 성공 (빈 main.cpp) + CoreTest 전체 통과

---

### Phase 1 — 도메인 구조체 & JsonRepository

**목표**: 데이터 계층 완성. 이 단계부터 JSON 파일에 읽고 쓸 수 있다.

#### 1-1. 구조체 정의

**Sample.h**
```cpp
struct Sample {
    int64_t     id;
    std::string name;
    double      avgProductionTime;  // 분
    double      yield;              // 0.0 ~ 1.0
    int         stock;

    static Sample   fromJson(const JsonValue& v);
    JsonValue       toJson() const;
};
```

**Order.h**
```cpp
enum class OrderStatus { Reserved, Rejected, Producing, Confirmed, Release };

struct Order {
    int64_t     id;
    int64_t     sampleId;
    std::string customerName;
    int         quantity;
    OrderStatus status;
    std::string createdAt;  // "YYYY-MM-DD HH:MM:SS"

    static Order       fromJson(const JsonValue& v);
    JsonValue          toJson() const;
    static OrderStatus fromString(const std::string& s);
    static std::string toString(OrderStatus s);
};
```

**ProductionJob.h**
```cpp
struct ProductionJob {
    int64_t orderId;
    int64_t sampleId;
    int     requiredAmount;      // 실생산량
    double  avgProdTimeMin;      // 단위당 평균 생산시간
    double  totalProdTimeMin;    // 총생산시간
    std::string startedAt;       // 큐 선두 진입 시각 (대기 중이면 "")
    int     queuePosition;       // 0 = 생산 중, 1+ = 대기

    static ProductionJob fromJson(const JsonValue& v);
    JsonValue            toJson() const;
};
```

#### 1-2. JsonRepository<T> 확장
- `DataPersistence`의 `JsonRepository<T>` → `Repository/JsonRepository.h` 복사
- `IRepository<T>` 상속 추가
- `T`가 `fromJson` / `toJson`을 가져야 한다는 concept 또는 주석 명시

#### 1-3. 단위 테스트 — `Test/RepositoryTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `Sample_JsonRoundTrip` | `Sample::fromJson(s.toJson()) == s` (모든 필드) |
| `Order_JsonRoundTrip` | `Order::fromJson(o.toJson()) == o` (status 문자열 변환 포함) |
| `ProductionJob_JsonRoundTrip` | `ProductionJob::fromJson(j.toJson()) == j` |
| `JsonRepository_Create` | create 후 findById로 동일 객체 반환 |
| `JsonRepository_Update` | update 후 findById로 변경된 필드 반환 |
| `JsonRepository_Remove` | remove 후 findById → nullopt |
| `JsonRepository_Persistence` | 파일 저장 후 새 인스턴스로 로드 시 데이터 유지 |

---

### Phase 2 — 시료 모델 & 시료 관리 기능

**목표**: 시료 등록·조회·검색 완성

#### 2-1. ISampleModel / SampleModel

```cpp
// Model/ISampleModel.h
class ISampleModel {
public:
    virtual ~ISampleModel() = default;
    virtual void                    add(Sample s)                           = 0;
    virtual std::optional<Sample>   findById(int64_t id) const              = 0;
    virtual std::vector<Sample>     getAll() const                          = 0;
    virtual std::vector<Sample>     searchByName(const std::string& kw) const = 0;
    virtual bool                    updateStock(int64_t id, int delta)      = 0;
};
```

- `SampleModel`은 내부적으로 `JsonRepository<Sample>` 보유
- `updateStock`: delta 양수 → 재고 증가, 음수 → 재고 차감 (0 미만 방지)

#### 2-2. ISampleView / SampleView
- `showSampleMenu()` — 1.등록 2.전체조회 3.검색 0.돌아가기
- `showSampleList(vector<Sample>)` — ID·이름·평균생산시간·수율·재고 테이블
- `promptNewSample()` → `Sample` 반환 (입력: 이름, 평균생산시간, 수율)
- `promptSearchKeyword()` → `string`
- `showMessage(string)`

#### 2-3. SampleController
```cpp
void SampleController::run() {
    while (true) {
        view_.showSampleMenu();
        switch (view_.getMenuChoice()) {
        case 1: handleRegister(); break;
        case 2: handleListAll();  break;
        case 3: handleSearch();   break;
        case 0: return;
        }
    }
}
```

#### 2-4. 단위 테스트 — `Test/SampleTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `SampleModel_Add_FindById` | 등록한 시료를 ID로 조회 |
| `SampleModel_GetAll` | 등록한 시료 수만큼 반환 |
| `SampleModel_SearchByName` | 부분 문자열로 필터링 |
| `SampleModel_UpdateStock_Increase` | delta 양수 → stock 증가 |
| `SampleModel_UpdateStock_Decrease` | delta 음수 → stock 차감 |
| `SampleModel_UpdateStock_NoBelowZero` | 차감 후 0 미만이면 false 반환, stock 불변 |
| `SampleController_Register` | MockView로 입력 흐름 + add 호출 1회 확인 |
| `SampleController_Search_NoResult` | 검색 결과 없을 때 showMessage 호출 확인 |

**완료 조건**: 시료 등록 후 프로그램 재시작 시 데이터 유지됨 + SampleTest 전체 통과

---

### Phase 3 — 주문 모델 & 주문 접수·승인·거절

**목표**: 주문 상태 전이 핵심 로직 완성

#### 3-1. IOrderModel / OrderModel

```cpp
class IOrderModel {
public:
    virtual int64_t                   add(Order o)                         = 0;
    virtual std::optional<Order>      findById(int64_t id) const           = 0;
    virtual std::vector<Order>        getAll() const                       = 0;
    virtual std::vector<Order>        getByStatus(OrderStatus s) const     = 0;
    virtual bool                      updateStatus(int64_t id, OrderStatus s) = 0;
};
```

#### 3-2. IOrderView / OrderView
- `showOrderMenu()` — 1.주문접수 2.예약목록 3.승인·거절 0.돌아가기
- `showOrderList(vector<Order>)`
- `promptNewOrder()` → `Order` (입력: sampleId, customerName, quantity)
- `promptOrderId()` → `int64_t`
- `showApprovalResult(Order, string reason)` (CONFIRMED or PRODUCING 안내)

#### 3-3. OrderController — 핵심 비즈니스 로직

**handleReserve()**
```
1. 시료 ID 존재 여부 확인 (ISampleModel::findById)
2. RESERVED 상태로 Order 생성 (createdAt = 현재시각)
3. 저장 및 결과 출력
```

**handleApprove()**
```
1. RESERVED 주문 목록 표시 → 주문 ID 입력
2. 재고 확인: sampleModel.findById(order.sampleId).stock
   ─ stock >= quantity:
       orderModel.updateStatus(id, Confirmed)
       sampleModel.updateStock(sampleId, -quantity)
   ─ stock < quantity:
       부족분 = quantity - stock
       실생산량 = ⌈부족분 / (yield × 0.9)⌉
       productionModel.enqueue(ProductionJob)
       orderModel.updateStatus(id, Producing)
3. 결과 출력
```

**handleReject()**
```
1. RESERVED 주문 목록 표시 → 주문 ID 입력
2. orderModel.updateStatus(id, Rejected)
```

#### 3-4. 단위 테스트 — `Test/OrderTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `OrderModel_Add_FindById` | 등록한 주문을 ID로 조회 |
| `OrderModel_GetByStatus` | RESERVED 필터링 정확도 |
| `OrderModel_UpdateStatus` | 상태 변경 후 findById로 확인 |
| `OrderController_Reserve_InvalidSample` | 존재하지 않는 sampleId → 에러 메시지 출력 |
| `OrderController_Approve_StockSufficient` | stock >= quantity → CONFIRMED, stock 차감 |
| `OrderController_Approve_StockInsufficient` | stock < quantity → PRODUCING, enqueue 1회 호출 |
| `OrderController_Approve_CalcRequiredAmount` | 실생산량 = ⌈부족분 / (yield × 0.9)⌉ 정확도 |
| `OrderController_Reject` | RESERVED → REJECTED 전환 |

**완료 조건**: 재고 충분 → CONFIRMED, 재고 부족 → PRODUCING + 생산 큐 등록 + OrderTest 전체 통과

---

### Phase 4 — 모니터링

**목표**: 상태별 주문 수·시료별 재고 현황 화면 완성

#### 4-1. IMonitorView / MonitorView
DataMonitor `Dashboard`의 박스 문자·CJK 너비 렌더링을 재사용한다.  
단, 독립 프로세스가 아닌 메뉴 진입 시 1회 렌더링 방식으로 변경한다.

**출력 레이아웃**
```
╔══════════════════════════════════════════════╗
║         모니터링 — 시스템 현황               ║
╠════════════════════╦═════════════════════════╣
║  ■ 주문 현황       ║  ■ 시료별 재고 현황     ║
║  RESERVED  :  N건  ║  알파-GaN  50   여유    ║
║  PRODUCING :  N건  ║  베타-SiC   0   고갈    ║
║  CONFIRMED :  N건  ║  감마-InP   3   부족    ║
║  RELEASE   :  N건  ║                         ║
╚════════════════════╩═════════════════════════╝
```

**재고 상태 판정**
- 고갈: `stock == 0`
- 부족: `0 < stock < CONFIRMED 주문의 quantity 합계`
- 여유: 그 외

#### 4-2. MonitorController
```cpp
void MonitorController::run() {
    auto orders  = orderModel_.getAll();
    auto samples = sampleModel_.getAll();
    view_.render(orders, samples);
    view_.waitKeyPress();
}
```

#### 4-3. 단위 테스트 — `Test/MonitorTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `MonitorController_StockStatus_Abundant` | stock > CONFIRMED 합계 → 여유 |
| `MonitorController_StockStatus_Shortage` | 0 < stock ≤ CONFIRMED 합계 → 부족 |
| `MonitorController_StockStatus_Depleted` | stock == 0 → 고갈 |
| `MonitorController_OrderCount_ByStatus` | 각 상태별 카운트 정확도 (REJECTED 제외 확인) |

**완료 조건**: 주문·시료 데이터 반영 정확도 확인 + MonitorTest 전체 통과

---

### Phase 5 — 생산라인

**목표**: FIFO 큐 관리, 진행 현황 출력, 완료 시 상태 전이

#### 5-1. IProductionModel / ProductionModel

```cpp
class IProductionModel {
public:
    virtual void                       enqueue(ProductionJob job)        = 0;
    virtual std::optional<ProductionJob> currentJob() const             = 0;
    virtual std::vector<ProductionJob>   waitingQueue() const           = 0;
    virtual int                         currentProducedAmount() const   = 0;
    virtual bool                        isCurrentComplete() const       = 0;
    virtual ProductionJob               dequeue()                       = 0;  // 완료 후 제거
};
```

**ProductionModel 내부 동작**
- `production_jobs.json`을 `JsonRepository<ProductionJob>`으로 관리
- 큐 선두(`queuePosition == 0`)에 `startedAt`이 없으면 현재 시각 기록 후 저장
- `currentProducedAmount()`: `(현재시각 - startedAt) / avgProdTimeMin` (정수 내림, 최대 requiredAmount)
- `isCurrentComplete()`: `currentProducedAmount() >= requiredAmount`

#### 5-2. IProductionView / ProductionView

**출력 레이아웃**
```
╔══════════════════════════════════════════════════════════════╗
║                   생산라인 현황                              ║
╠══════════════════════════════════════════════════════════════╣
║  [현재 생산 중]                                              ║
║  주문 #3 | 알파-GaN (수율: 85.0% / 생산시간: 30분) | 고객: 삼성전자  ║
║  주문량: 30개 | 재고: 10개 | 부족분: 20개 | 실생산량: 12개  ║
║  진행: ████████░░ 8/12개  완료 예정: 09:38                  ║
╠══════════════════════════════════════════════════════════════╣
║  [대기 큐]                                                   ║
║  순번  주문#  시료        주문량  부족분  실생산량  예정 완료 ║
║  ────  ─────  ──────────  ──────  ──────  ────────  ────────  ║
║    1   #5     베타-SiC    50개    30개    34개      11:58    ║
║    2   #7     감마-InP    15개    15개     9개      14:27    ║
╚══════════════════════════════════════════════════════════════╝
  [C] 완료 처리  [0] 돌아가기
```

> **완료 예정 시각 계산**:  
> `현재시각 + (앞 작업들의 totalProdTimeMin 누적합) + 자신의 totalProdTimeMin` → `HH:MM` 표기  
> (예: 순번 2의 완료 예정 = 현재시각 + 현재작업 남은시간 + 순번 1의 totalProdTimeMin + 자신의 totalProdTimeMin)

#### 5-3. ProductionController

```cpp
void ProductionController::run() {
    // 화면 렌더링
    view_.render(productionModel_.currentJob(),
                 productionModel_.currentProducedAmount(),
                 productionModel_.waitingQueue());

    // 완료 처리 입력 처리
    if (view_.isCompleteRequested() && productionModel_.isCurrentComplete()) {
        auto job = productionModel_.dequeue();
        sampleModel_.updateStock(job.sampleId, job.requiredAmount);
        orderModel_.updateStatus(job.orderId, OrderStatus::Confirmed);
        // 다음 큐 선두 startedAt 초기화는 다음 currentJob() 호출 시 자동
    }
}
```

> **생산 완료 판단**: 사용자가 생산라인 메뉴 진입 시 자동으로 완료 여부 계산.  
> 완료 상태이면 화면에 "완료 처리 가능" 표시 후 `C` 키로 처리.

#### 5-4. 단위 테스트 — `Test/ProductionTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `ProductionModel_Enqueue_FIFO` | 3건 enqueue 후 currentJob이 첫 번째 작업 |
| `ProductionModel_WaitingQueue_Order` | waitingQueue가 enqueue 순서와 동일 |
| `ProductionModel_CurrentProducedAmount` | 경과 시간 기반 생산량 계산 정확도 |
| `ProductionModel_IsComplete_False` | 경과 < 총생산시간 → false |
| `ProductionModel_IsComplete_True` | 경과 ≥ 총생산시간 → true |
| `ProductionModel_Dequeue_PromotesNext` | dequeue 후 다음 작업이 currentJob으로 승격 |
| `ProductionController_Complete_UpdatesStock` | 완료 시 sampleModel.updateStock 호출 확인 |
| `ProductionController_Complete_UpdatesOrderStatus` | 완료 시 orderModel.updateStatus(Confirmed) 호출 확인 |

**완료 조건**: 승인 후 PRODUCING 주문이 생산 큐에 등록되고, 생산 완료 후 CONFIRMED로 전환됨 + ProductionTest 전체 통과

---

### Phase 6 — 출고 처리

**목표**: CONFIRMED 주문 목록 조회 후 선택적 RELEASE 처리

#### 6-1. IShipmentView / ShipmentView
- `showConfirmedOrders(vector<Order>)`
- `promptOrderId()` → `int64_t`
- `showReleaseResult(Order)`

#### 6-2. ShipmentController
```cpp
void ShipmentController::handleRelease() {
    auto confirmed = orderModel_.getByStatus(OrderStatus::Confirmed);
    view_.showConfirmedOrders(confirmed);
    int64_t id = view_.promptOrderId();
    orderModel_.updateStatus(id, OrderStatus::Release);
    view_.showReleaseResult(*orderModel_.findById(id));
}
```

#### 6-3. 단위 테스트 — `Test/ShipmentTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `ShipmentController_Release_StatusChange` | CONFIRMED → RELEASE 전환 |
| `ShipmentController_Release_InvalidId` | CONFIRMED 아닌 주문 ID 입력 → 에러 메시지 출력 |

**완료 조건**: CONFIRMED → RELEASE 전환 후 JSON 파일 반영 확인 + ShipmentTest 전체 통과

---

### Phase 7 — 메인 메뉴 & 전체 통합

**목표**: 모든 컨트롤러를 AppController로 연결, 메인 메뉴 시스템 요약 표시

#### 7-1. SystemSummary 구조체
```cpp
struct SystemSummary {
    int totalSamples;
    int reservedCount;
    int producingCount;
    int confirmedCount;
    int releaseCount;
};
```

#### 7-2. IMenuView / MenuView

**메인 메뉴 레이아웃**
```
╔══════════════════════════════════════════════╗
║    반도체 시료 생산주문관리 시스템           ║
╠══════════════════════════════════════════════╣
║  시스템 현황: 2026-06-12  09:30:00           ║
╠══════════════════════════════════════════════╣
║  등록시료: 5종      총재고: 80개            ║
║  전체주문: 7건      생산라인: 2건           ║
╠══════════════════════════════════════════════╣
║  1. 시료 관리                                ║
║  2. 주문 등록                               ║
║  3. 주문 승인 / 거절                        ║
║  4. 모니터링                                ║
║  5. 생산라인 조회                           ║
║  6. 출고 처리                               ║
║  0. 종료                                    ║
╚══════════════════════════════════════════════╝
```

#### 7-3. AppController
```cpp
void AppController::run() {
    while (true) {
        SystemSummary summary = buildSummary();
        view_.showMainMenu(summary);
        switch (view_.getMenuChoice()) {
        case 1: sampleCtrl_.run();          break;
        case 2: orderCtrl_.runReserve();    break;
        case 3: orderCtrl_.runApproval();   break;
        case 4: monitorCtrl_.run();         break;
        case 5: shipmentCtrl_.run();        break;
        case 6: productionCtrl_.run();      break;
        case 0: return;
        }
    }
}
```

#### 7-4. main.cpp 의존성 주입

```cpp
int main() {
    SetConsoleOutputCP(65001);

    // Core 유틸 경로 설정
    std::filesystem::path dataDir = "data";
    std::filesystem::create_directories(dataDir);

    // Repository 생성
    JsonRepository<Sample>        sampleRepo(dataDir / "samples.json");
    JsonRepository<Order>         orderRepo(dataDir / "orders.json");
    JsonRepository<ProductionJob> prodRepo(dataDir / "production_jobs.json");

    // Model 생성
    SampleModel     sampleModel(sampleRepo);
    OrderModel      orderModel(orderRepo);
    ProductionModel prodModel(prodRepo);

    // View 생성
    MenuView       menuView;
    SampleView     sampleView;
    OrderView      orderView;
    MonitorView    monitorView;
    ProductionView prodView;
    ShipmentView   shipmentView;

    // Controller 생성
    SampleController     sampleCtrl(sampleModel, sampleView);
    OrderController      orderCtrl(orderModel, sampleModel, prodModel, orderView);
    MonitorController    monitorCtrl(orderModel, sampleModel, monitorView);
    ProductionController prodCtrl(prodModel, orderModel, sampleModel, prodView);
    ShipmentController   shipmentCtrl(orderModel, shipmentView);
    AppController        app(sampleModel, orderModel, menuView,
                             sampleCtrl, orderCtrl, monitorCtrl,
                             prodCtrl, shipmentCtrl);

    app.run();
    return 0;
}
```

#### 7-5. 단위 테스트 — `Test/AppControllerTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `AppController_BuildSummary_SampleCount` | totalSamples = sampleModel.getAll().size() |
| `AppController_BuildSummary_OrderCounts` | 상태별 카운트 (REJECTED 제외) |
| `AppController_BuildSummary_ProductionCount` | waitingQueue + currentJob 합산 |
| `AppController_Routing_Case1` | 메뉴 1 → sampleCtrl.run() 1회 호출 |
| `AppController_Routing_Case2` | 메뉴 2 → orderCtrl.runReserve() 1회 호출 |
| `AppController_Routing_Case0` | 메뉴 0 → 루프 종료 |

**완료 조건**: 전체 시나리오(주문→승인→생산→완료→출고) end-to-end 동작 확인 + AppControllerTest 전체 통과

---

### Phase 8 — 통합 테스트

**목표**: 실제 JSON 파일을 사용한 계층 간 end-to-end 시나리오 자동 검증  
(단위 테스트는 각 Phase 완료 조건에 포함. Phase 8은 Mock 없이 실제 구현체 사용)

#### 8-1. Mock 클래스 (각 Phase 테스트에서 공용)
```cpp
// Test/Mocks.h
class MockSampleModel    : public ISampleModel    { /* MOCK_METHOD */ };
class MockOrderModel     : public IOrderModel     { ... };
class MockProductionModel: public IProductionModel{ ... };
class MockSampleView     : public ISampleView     { ... };
class MockOrderView      : public IOrderView      { ... };
class MockProductionView : public IProductionView { ... };
class MockShipmentView   : public IShipmentView   { ... };
```

#### 8-2. 통합 테스트 케이스 — `Test/IntegrationTest.cpp`

| 테스트 | 검증 내용 |
|--------|---------|
| `Scenario_A_StockSufficient` | 시료 등록 → 주문 → 승인(재고 충분) → 출고, 최종 status == Release |
| `Scenario_B_StockInsufficient` | 시료 등록 → 주문 → 승인(재고 부족) → 생산완료 → 출고, 최종 status == Release |
| `Scenario_C_Reject` | 주문 → 거절, status == Rejected + 모니터링 카운트 미포함 |
| `Scenario_D_MultipleFIFO` | 재고 부족 주문 3건 승인 → 큐 순서 검증 → 순서대로 완료 처리 |
| `Scenario_Persistence` | 시나리오 A 실행 후 프로세스 재시작(재로드) → 데이터 일관성 유지 |

#### 8-3. 테스트 프로젝트 구성
- `SampleOrderSystemTest` vcxproj 별도 생성
- NuGet `gmock.1.11.0` 참조
- 테스트 실행 시 임시 디렉토리(`Test/tmp/`) 사용 → 테스트 종료 후 자동 삭제

---

### Phase 9 — 더미 데이터 & 최종 검증

**목표**: 실제 데이터로 전체 워크플로우 검증

#### 9-1. DummyDataGenerator 활용
- 별도 실행: `DummyDataGenerator` → `data/samples.json`, `data/orders.json` 생성
- 생성 설정 권고: 시료 5~10개, 주문 20~30개 (상태 분포 자동 적용)

#### 9-2. 검증 시나리오

```
시나리오 A — 정상 흐름 (재고 충분)
  1. 시료 등록 (stock >= quantity)
  2. 주문 접수 → RESERVED
  3. 주문 승인 → CONFIRMED (재고 차감 확인)
  4. 출고 처리 → RELEASE

시나리오 B — 생산 흐름 (재고 부족)
  1. 시료 등록 (stock < quantity)
  2. 주문 접수 → RESERVED
  3. 주문 승인 → PRODUCING + 생산 큐 등록
  4. 생산라인 화면 → 진행도 확인
  5. 생산 완료 처리 → CONFIRMED (재고 증가 확인)
  6. 출고 처리 → RELEASE

시나리오 C — 거절 흐름
  1. 주문 접수 → RESERVED
  2. 주문 거절 → REJECTED (모니터링에 미표시 확인)

시나리오 D — 복수 생산 대기
  1. 재고 부족 주문 3건 연속 승인
  2. 생산 큐에 3건 FIFO 순서 확인
  3. 순서대로 완료 처리 확인
```
