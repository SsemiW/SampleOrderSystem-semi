#include "ShipmentController.h"

ShipmentController::ShipmentController(IOrderModel& orderModel, IShipmentView& view)
    : orderModel_(orderModel), view_(view) {}

void ShipmentController::run() {
    handleRelease();
}

void ShipmentController::handleRelease() {
    auto confirmed = orderModel_.getByStatus(OrderStatus::Confirmed);
    view_.showConfirmedOrders(confirmed);
    int64_t id = view_.promptOrderId();

    auto opt = orderModel_.findById(id);
    if (!opt || opt->status != OrderStatus::Confirmed) {
        view_.showMessage("유효하지 않은 주문 ID입니다.");
        return;
    }

    orderModel_.updateStatus(id, OrderStatus::Release);
    view_.showReleaseResult(*orderModel_.findById(id));
}
