#include "MonitorController.h"

MonitorController::MonitorController(IOrderModel& orderModel, ISampleModel& sampleModel,
                                     IMonitorView& view)
    : orderModel_(orderModel), sampleModel_(sampleModel), view_(view) {}

void MonitorController::run() {
    view_.render(orderModel_.getAll(), sampleModel_.getAll());
    view_.waitKeyPress();
}

std::string MonitorController::stockLabel(const Sample& s,
                                          const std::vector<Order>& allOrders) {
    int confirmedQty = 0;
    for (const auto& o : allOrders)
        if (o.status == OrderStatus::Confirmed && o.sampleId == s.id)
            confirmedQty += o.quantity;

    if (s.stock == 0)             return "고갈";
    if (s.stock < confirmedQty)   return "부족";
    return "여유";
}
