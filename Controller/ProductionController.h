#pragma once
#include "IProductionController.h"
#include "../Model/IProductionModel.h"
#include "../Model/IOrderModel.h"
#include "../Model/ISampleModel.h"
#include "../View/IProductionView.h"

class ProductionController : public IProductionController {
public:
    ProductionController(IProductionModel& prodModel,
                         IOrderModel&      orderModel,
                         ISampleModel&     sampleModel,
                         IProductionView&  view);
    void run() override;

private:
    IProductionModel& prodModel_;
    IOrderModel&      orderModel_;
    ISampleModel&     sampleModel_;
    IProductionView&  view_;
};
