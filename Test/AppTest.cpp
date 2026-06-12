#include <gtest/gtest.h>
#include "Test/Mocks.h"
#include "Controller/AppController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::AnyNumber;

struct AppControllerTest : ::testing::Test {
    MockSampleModel          sampleModel;
    MockOrderModel           orderModel;
    MockSampleController     sample;
    MockOrderController      order;
    MockProductionController production;
    MockShipmentController   shipment;
    MockMonitorController    monitor;
    MockMenuView             view;

    AppController make() {
        return AppController(sample, order, production, shipment, monitor, view,
                             sampleModel, orderModel);
    }

    // computeStats() 호출에 필요한 모델 스텁 설정
    void stubStats() {
        EXPECT_CALL(sampleModel, getAll())
            .WillRepeatedly(Return(std::vector<Sample>{}));
        EXPECT_CALL(orderModel, getAll())
            .WillRepeatedly(Return(std::vector<Order>{}));
        EXPECT_CALL(orderModel, getByStatus(OrderStatus::Producing))
            .WillRepeatedly(Return(std::vector<Order>{}));
    }
};

// 선택 0 → 즉시 종료
TEST_F(AppControllerTest, ExitOnChoiceZero) {
    stubStats();
    EXPECT_CALL(view, showMainMenu(_)).Times(1);
    EXPECT_CALL(view, getMenuChoice()).WillOnce(Return(0));

    auto app = make();
    app.run();
}

// 선택 1 → sample.run() 호출
TEST_F(AppControllerTest, Choice1_CallsSampleRun) {
    stubStats();
    EXPECT_CALL(view, showMainMenu(_)).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(sample, run()).Times(1);

    auto app = make();
    app.run();
}

// 선택 2 → order.runReserve() 호출
TEST_F(AppControllerTest, Choice2_CallsOrderReserve) {
    stubStats();
    EXPECT_CALL(view, showMainMenu(_)).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(2))
        .WillOnce(Return(0));
    EXPECT_CALL(order, runReserve()).Times(1);

    auto app = make();
    app.run();
}

// 선택 3 → order.runApproval() 호출
TEST_F(AppControllerTest, Choice3_CallsOrderApproval) {
    stubStats();
    EXPECT_CALL(view, showMainMenu(_)).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(3))
        .WillOnce(Return(0));
    EXPECT_CALL(order, runApproval()).Times(1);

    auto app = make();
    app.run();
}

// 선택 4 → monitor.run() 호출
TEST_F(AppControllerTest, Choice4_CallsMonitorRun) {
    stubStats();
    EXPECT_CALL(view, showMainMenu(_)).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(4))
        .WillOnce(Return(0));
    EXPECT_CALL(monitor, run()).Times(1);

    auto app = make();
    app.run();
}

// 선택 5 → production.run() 호출
TEST_F(AppControllerTest, Choice5_CallsProductionRun) {
    stubStats();
    EXPECT_CALL(view, showMainMenu(_)).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(5))
        .WillOnce(Return(0));
    EXPECT_CALL(production, run()).Times(1);

    auto app = make();
    app.run();
}

// 선택 6 → shipment.run() 호출
TEST_F(AppControllerTest, Choice6_CallsShipmentRun) {
    stubStats();
    EXPECT_CALL(view, showMainMenu(_)).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(6))
        .WillOnce(Return(0));
    EXPECT_CALL(shipment, run()).Times(1);

    auto app = make();
    app.run();
}
