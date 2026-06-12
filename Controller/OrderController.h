#pragma once
#include "../Model/IOrderModel.h"
#include "../Model/ISampleModel.h"
#include "../Model/IProductionModel.h"
#include "../View/IOrderView.h"

class OrderController {
public:
    OrderController(IOrderModel& orderModel, ISampleModel& sampleModel,
                    IProductionModel& productionModel, IOrderView& view);
    void runReserve();
    void runApproval();

private:
    void handleApprove();
    void handleReject();

    IOrderModel&      orderModel_;
    ISampleModel&     sampleModel_;
    IProductionModel& productionModel_;
    IOrderView&       view_;
};
