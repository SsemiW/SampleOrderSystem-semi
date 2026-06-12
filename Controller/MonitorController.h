#pragma once
#include <vector>
#include <string>
#include "IMonitorController.h"
#include "../Model/IOrderModel.h"
#include "../Model/ISampleModel.h"
#include "../View/IMonitorView.h"

class MonitorController : public IMonitorController {
public:
    MonitorController(IOrderModel& orderModel, ISampleModel& sampleModel,
                      IMonitorView& view);
    void run() override;

    // Phase 4-3 단위 테스트에서 직접 검증용
    static std::string stockLabel(const Sample& s,
                                  const std::vector<Order>& allOrders);

private:
    IOrderModel&  orderModel_;
    ISampleModel& sampleModel_;
    IMonitorView& view_;
};
