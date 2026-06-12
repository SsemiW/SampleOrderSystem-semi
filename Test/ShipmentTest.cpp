#include <gtest/gtest.h>
#include "Test/Mocks.h"
#include "Controller/ShipmentController.h"

using ::testing::_;
using ::testing::Return;

class ShipmentControllerTest : public ::testing::Test {
protected:
    MockOrderModel   orderModel;
    MockShipmentView view;

    Order makeOrder(int64_t id, OrderStatus status) {
        Order o;
        o.id           = id;
        o.sampleId     = 1;
        o.customerName = "테스트고객";
        o.quantity     = 10;
        o.status       = status;
        o.createdAt    = "2026-06-12 09:00:00";
        return o;
    }
};

TEST_F(ShipmentControllerTest, Release_StatusChange) {
    Order confirmed = makeOrder(1, OrderStatus::Confirmed);
    Order released  = makeOrder(1, OrderStatus::Release);

    EXPECT_CALL(orderModel, getByStatus(OrderStatus::Confirmed))
        .WillOnce(Return(std::vector<Order>{confirmed}));
    EXPECT_CALL(view, showConfirmedOrders(_)).Times(1);
    EXPECT_CALL(view, promptOrderId()).WillOnce(Return(1L));
    EXPECT_CALL(orderModel, findById(1L))
        .WillOnce(Return(confirmed))
        .WillOnce(Return(released));
    EXPECT_CALL(orderModel, updateStatus(1L, OrderStatus::Release)).Times(1);
    EXPECT_CALL(view, showReleaseResult(released)).Times(1);

    ShipmentController ctrl(orderModel, view);
    ctrl.run();
}

TEST_F(ShipmentControllerTest, Release_InvalidId) {
    Order confirmed = makeOrder(1, OrderStatus::Confirmed);

    EXPECT_CALL(orderModel, getByStatus(OrderStatus::Confirmed))
        .WillOnce(Return(std::vector<Order>{confirmed}));
    EXPECT_CALL(view, showConfirmedOrders(_)).Times(1);
    EXPECT_CALL(view, promptOrderId()).WillOnce(Return(999L));
    EXPECT_CALL(orderModel, findById(999L)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(view, showMessage("유효하지 않은 주문 ID입니다.")).Times(1);

    ShipmentController ctrl(orderModel, view);
    ctrl.run();
}
