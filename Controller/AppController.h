#pragma once
#include "ISampleController.h"
#include "IOrderController.h"
#include "IProductionController.h"
#include "IShipmentController.h"
#include "IMonitorController.h"
#include "../View/IMenuView.h"
#include "../Model/ISampleModel.h"
#include "../Model/IOrderModel.h"

class AppController {
public:
    AppController(ISampleController&     sample,
                  IOrderController&      order,
                  IProductionController& production,
                  IShipmentController&   shipment,
                  IMonitorController&    monitor,
                  IMenuView&             view,
                  ISampleModel&          sampleModel,
                  IOrderModel&           orderModel);
    void run();

private:
    MenuStats computeStats() const;

    ISampleController&     sample_;
    IOrderController&      order_;
    IProductionController& production_;
    IShipmentController&   shipment_;
    IMonitorController&    monitor_;
    IMenuView&             view_;
    ISampleModel&          sampleModel_;
    IOrderModel&           orderModel_;
};
