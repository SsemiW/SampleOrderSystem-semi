#include <gtest/gtest.h>
#include "Test/Mocks.h"
#include "Controller/AppController.h"

using ::testing::Return;
using ::testing::_;

struct AppControllerTest : ::testing::Test {
    MockSampleController     sample;
    MockOrderController      order;
    MockProductionController production;
    MockShipmentController   shipment;
    MockMonitorController    monitor;
    MockMenuView             view;

    AppController make() {
        return AppController(sample, order, production, shipment, monitor, view);
    }
};

// 역할 0 선택 시 즉시 종료
TEST_F(AppControllerTest, ExitOnRoleZero) {
    EXPECT_CALL(view, selectRole()).WillOnce(Return(0));

    auto app = make();
    app.run();
}

// 고객 메뉴 → 시료 관리(1) → sample.run() 호출
TEST_F(AppControllerTest, Customer_CallsSampleRun) {
    EXPECT_CALL(view, selectRole())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(view, showCustomerMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(sample, run()).Times(1);

    auto app = make();
    app.run();
}

// 고객 메뉴 → 주문 접수(2) → order.runReserve() 호출
TEST_F(AppControllerTest, Customer_CallsOrderReserve) {
    EXPECT_CALL(view, selectRole())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(view, showCustomerMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(2))
        .WillOnce(Return(0));
    EXPECT_CALL(order, runReserve()).Times(1);

    auto app = make();
    app.run();
}

// 주문 담당자 메뉴 → 주문 승인/거절(1) → order.runApproval() 호출
TEST_F(AppControllerTest, OrderManager_CallsOrderApproval) {
    EXPECT_CALL(view, selectRole())
        .WillOnce(Return(2))
        .WillOnce(Return(0));
    EXPECT_CALL(view, showOrderManagerMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(order, runApproval()).Times(1);

    auto app = make();
    app.run();
}

// 주문 담당자 메뉴 → 모니터링(2) → monitor.run() 호출
TEST_F(AppControllerTest, OrderManager_CallsMonitorRun) {
    EXPECT_CALL(view, selectRole())
        .WillOnce(Return(2))
        .WillOnce(Return(0));
    EXPECT_CALL(view, showOrderManagerMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(2))
        .WillOnce(Return(0));
    EXPECT_CALL(monitor, run()).Times(1);

    auto app = make();
    app.run();
}

// 생산 담당자 메뉴 → 생산 라인 운영(1) → production.run() 호출
TEST_F(AppControllerTest, ProductionManager_CallsProductionRun) {
    EXPECT_CALL(view, selectRole())
        .WillOnce(Return(3))
        .WillOnce(Return(0));
    EXPECT_CALL(view, showProductionManagerMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(production, run()).Times(1);

    auto app = make();
    app.run();
}

// 생산 담당자 메뉴 → 출고 처리(2) → shipment.run() 호출
TEST_F(AppControllerTest, ProductionManager_CallsShipmentRun) {
    EXPECT_CALL(view, selectRole())
        .WillOnce(Return(3))
        .WillOnce(Return(0));
    EXPECT_CALL(view, showProductionManagerMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(2))
        .WillOnce(Return(0));
    EXPECT_CALL(shipment, run()).Times(1);

    auto app = make();
    app.run();
}

// 생산 담당자 메뉴 → 모니터링(3) → monitor.run() 호출
TEST_F(AppControllerTest, ProductionManager_CallsMonitorRun) {
    EXPECT_CALL(view, selectRole())
        .WillOnce(Return(3))
        .WillOnce(Return(0));
    EXPECT_CALL(view, showProductionManagerMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(3))
        .WillOnce(Return(0));
    EXPECT_CALL(monitor, run()).Times(1);

    auto app = make();
    app.run();
}
