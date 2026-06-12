#pragma once
#include "ISampleController.h"
#include "IOrderController.h"
#include "IProductionController.h"
#include "IShipmentController.h"
#include "IMonitorController.h"
#include "../View/IMenuView.h"

class AppController {
public:
    AppController(ISampleController&     sample,
                  IOrderController&      order,
                  IProductionController& production,
                  IShipmentController&   shipment,
                  IMonitorController&    monitor,
                  IMenuView&             view);
    void run();

private:
    void runCustomer();
    void runOrderManager();
    void runProductionManager();

    ISampleController&     sample_;
    IOrderController&      order_;
    IProductionController& production_;
    IShipmentController&   shipment_;
    IMonitorController&    monitor_;
    IMenuView&             view_;
};
