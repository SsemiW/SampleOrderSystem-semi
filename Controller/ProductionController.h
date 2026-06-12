#pragma once
#include "../Model/IProductionModel.h"
#include "../Model/IOrderModel.h"
#include "../Model/ISampleModel.h"
#include "../View/IProductionView.h"

class ProductionController {
public:
    ProductionController(IProductionModel& prodModel,
                         IOrderModel&      orderModel,
                         ISampleModel&     sampleModel,
                         IProductionView&  view);
    void run();

private:
    IProductionModel& prodModel_;
    IOrderModel&      orderModel_;
    ISampleModel&     sampleModel_;
    IProductionView&  view_;
};
