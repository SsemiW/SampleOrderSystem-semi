#include "AppController.h"
#include <chrono>
#include <ctime>

AppController::AppController(ISampleController&     sample,
                             IOrderController&      order,
                             IProductionController& production,
                             IShipmentController&   shipment,
                             IMonitorController&    monitor,
                             IMenuView&             view,
                             ISampleModel&          sampleModel,
                             IOrderModel&           orderModel)
    : sample_(sample), order_(order), production_(production),
      shipment_(shipment), monitor_(monitor), view_(view),
      sampleModel_(sampleModel), orderModel_(orderModel) {}

void AppController::run() {
    while (true) {
        view_.showMainMenu(computeStats());
        switch (view_.getMenuChoice()) {
        case 1: sample_.run();         break;
        case 2: order_.runReserve();   break;
        case 3: order_.runApproval();  break;
        case 4: monitor_.run();        break;
        case 5: production_.run();     break;
        case 6: shipment_.run();       break;
        case 0: return;
        }
    }
}

MenuStats AppController::computeStats() const {
    MenuStats s;

    auto now = std::chrono::system_clock::now();
    auto tt  = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &tt);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d  %H:%M:%S", &tm);
    s.datetime = buf;

    const auto samples = sampleModel_.getAll();
    s.sampleCount = static_cast<int>(samples.size());
    for (const auto& sample : samples)
        s.totalStock += sample.stock;

    s.orderCount      = static_cast<int>(orderModel_.getAll().size());
    s.productionCount = static_cast<int>(
        orderModel_.getByStatus(OrderStatus::Producing).size());

    return s;
}
