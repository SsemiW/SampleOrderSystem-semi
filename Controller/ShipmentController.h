#pragma once
#include "../Model/IOrderModel.h"
#include "../View/IShipmentView.h"

class ShipmentController {
public:
    ShipmentController(IOrderModel& orderModel, IShipmentView& view);
    void run();

private:
    void handleRelease();

    IOrderModel&   orderModel_;
    IShipmentView& view_;
};
