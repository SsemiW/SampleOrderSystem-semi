#pragma once
#include "IOrderController.h"
#include "../Model/IOrderModel.h"
#include "../Model/ISampleModel.h"
#include "../Model/IProductionModel.h"
#include "../View/IOrderView.h"

class OrderController : public IOrderController {
public:
    OrderController(IOrderModel& orderModel, ISampleModel& sampleModel,
                    IProductionModel& productionModel, IOrderView& view);
    void runReserve()  override;
    void runApproval() override;

private:
    void handleApprove();
    void handleReject();

    IOrderModel&      orderModel_;
    ISampleModel&     sampleModel_;
    IProductionModel& productionModel_;
    IOrderView&       view_;
};
