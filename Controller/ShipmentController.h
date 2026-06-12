#pragma once
#include "IShipmentController.h"
#include "../Model/IOrderModel.h"
#include "../View/IShipmentView.h"

class ShipmentController : public IShipmentController {
public:
    ShipmentController(IOrderModel& orderModel, IShipmentView& view);
    void run() override;

private:
    void handleRelease();

    IOrderModel&   orderModel_;
    IShipmentView& view_;
};
