#include "OrderController.h"
#include <chrono>
#include <ctime>
#include <cmath>

namespace {
std::string currentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto tt  = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &tt);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}
}

OrderController::OrderController(IOrderModel& orderModel, ISampleModel& sampleModel,
                                 IProductionModel& productionModel, IOrderView& view)
    : orderModel_(orderModel), sampleModel_(sampleModel),
      productionModel_(productionModel), view_(view) {}

void OrderController::runReserve() {
    while (true) {
        Order o = view_.promptNewOrder();
        if (!sampleModel_.findById(o.sampleId)) {
            view_.showMessage("존재하지 않는 시료 ID입니다.");
            continue;
        }
        o.status    = OrderStatus::Reserved;
        o.createdAt = currentTimeString();
        orderModel_.add(o);
        view_.showMessage("주문이 접수되었습니다.");
        return;
    }
}

void OrderController::runApproval() {
    while (true) {
        auto reserved = orderModel_.getByStatus(OrderStatus::Reserved);
        view_.showOrderList(reserved);
        view_.showOrderMenu();
        int choice = view_.getMenuChoice();
        switch (choice) {
        case 1: handleApprove(); break;
        case 2: handleReject();  break;
        case 0: return;
        default: break;
        }
    }
}

void OrderController::handleApprove() {
    int64_t id = view_.promptOrderId();
    auto opt = orderModel_.findById(id);
    if (!opt || opt->status != OrderStatus::Reserved) {
        view_.showMessage("유효하지 않은 주문 ID입니다.");
        return;
    }
    auto sampleOpt = sampleModel_.findById(opt->sampleId);
    if (!sampleOpt) {
        view_.showMessage("연결된 시료를 찾을 수 없습니다.");
        return;
    }

    if (sampleOpt->stock >= opt->quantity) {
        orderModel_.updateStatus(id, OrderStatus::Confirmed);
        sampleModel_.updateStock(opt->sampleId, -opt->quantity);
        view_.showApprovalResult(*orderModel_.findById(id), "재고 충분 - 확정");
    } else {
        int shortage = opt->quantity - sampleOpt->stock;
        int required = static_cast<int>(
            std::ceil(static_cast<double>(shortage) / (sampleOpt->yield * 0.9)));

        ProductionJob job;
        job.orderId          = opt->id;
        job.sampleId         = opt->sampleId;
        job.requiredAmount   = required;
        job.avgProdTimeMin   = sampleOpt->avgProductionTime;
        job.totalProdTimeMin = sampleOpt->avgProductionTime * required;
        job.startedAt        = "";
        job.queuePosition    = 0;

        productionModel_.enqueue(job);
        orderModel_.updateStatus(id, OrderStatus::Producing);
        view_.showApprovalResult(*orderModel_.findById(id), "재고 부족 - 생산 중");
    }
}

void OrderController::handleReject() {
    int64_t id = view_.promptOrderId();
    auto opt = orderModel_.findById(id);
    if (!opt || opt->status != OrderStatus::Reserved) {
        view_.showMessage("유효하지 않은 주문 ID입니다.");
        return;
    }
    orderModel_.remove(id);
    view_.showMessage("주문이 거절되었습니다.");
}
