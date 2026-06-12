#include <gtest/gtest.h>
#include "Test/Mocks.h"
#include "Controller/MonitorController.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SaveArg;

namespace {

Sample makeSample(int64_t id, int stock) {
    Sample s;
    s.id = id; s.name = "테스트시료";
    s.avgProductionTime = 30.0; s.yield = 0.85; s.stock = stock;
    return s;
}

Order makeOrder(int64_t sampleId, int qty, OrderStatus status) {
    Order o;
    o.id = sampleId; o.sampleId = sampleId;
    o.customerName = "고객"; o.quantity = qty;
    o.status = status; o.createdAt = "2026-06-12 09:00:00";
    return o;
}

} // namespace

// ─── stockLabel 직접 검증 ─────────────────────────────────────────────────

TEST(MonitorControllerTest, StockStatus_Abundant) {
    // stock(100) > CONFIRMED 합계(30) → 여유
    Sample s = makeSample(1, 100);
    std::vector<Order> orders = {
        makeOrder(1, 30, OrderStatus::Confirmed)
    };
    EXPECT_EQ(MonitorController::stockLabel(s, orders), "여유");
}

TEST(MonitorControllerTest, StockStatus_Shortage) {
    // 0 < stock(10) < CONFIRMED 합계(30) → 부족
    Sample s = makeSample(1, 10);
    std::vector<Order> orders = {
        makeOrder(1, 30, OrderStatus::Confirmed)
    };
    EXPECT_EQ(MonitorController::stockLabel(s, orders), "부족");
}

TEST(MonitorControllerTest, StockStatus_Depleted) {
    // stock == 0 → 고갈 (CONFIRMED 합계 무관)
    Sample s = makeSample(1, 0);
    std::vector<Order> orders = {
        makeOrder(1, 30, OrderStatus::Confirmed)
    };
    EXPECT_EQ(MonitorController::stockLabel(s, orders), "고갈");
}

// ─── 상태별 카운트 정확도 (run() 통합) ───────────────────────────────────

TEST(MonitorControllerTest, OrderCount_ByStatus) {
    MockOrderModel  orderModel;
    MockSampleModel sampleModel;
    MockMonitorView view;

    // RESERVED 2건, PRODUCING 1건, CONFIRMED 1건, RELEASE 1건, REJECTED 1건
    std::vector<Order> orders = {
        makeOrder(1, 10, OrderStatus::Reserved),
        makeOrder(1, 10, OrderStatus::Reserved),
        makeOrder(1, 10, OrderStatus::Producing),
        makeOrder(1, 10, OrderStatus::Confirmed),
        makeOrder(1, 10, OrderStatus::Release),
        makeOrder(1, 10, OrderStatus::Rejected)
    };

    EXPECT_CALL(orderModel,  getAll()).WillOnce(Return(orders));
    EXPECT_CALL(sampleModel, getAll()).WillOnce(Return(std::vector<Sample>{}));
    EXPECT_CALL(view, waitKeyPress()).Times(1);

    std::vector<Order> captured;
    EXPECT_CALL(view, render(_, _)).WillOnce(SaveArg<0>(&captured));

    MonitorController ctrl(orderModel, sampleModel, view);
    ctrl.run();

    // 전체 주문 6건이 뷰로 전달됨
    ASSERT_EQ(captured.size(), 6u);

    // 상태별 카운트 (뷰에서 REJECTED는 제외)
    int nRes = 0, nPro = 0, nCon = 0, nRel = 0, nRej = 0;
    for (const auto& o : captured) {
        switch (o.status) {
        case OrderStatus::Reserved:  ++nRes; break;
        case OrderStatus::Producing: ++nPro; break;
        case OrderStatus::Confirmed: ++nCon; break;
        case OrderStatus::Release:   ++nRel; break;
        case OrderStatus::Rejected:  ++nRej; break;
        }
    }
    EXPECT_EQ(nRes, 2);
    EXPECT_EQ(nPro, 1);
    EXPECT_EQ(nCon, 1);
    EXPECT_EQ(nRel, 1);
    EXPECT_EQ(nRej, 1);  // REJECTED는 뷰로 전달되나 렌더링에서 제외됨
}
