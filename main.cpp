#include <Windows.h>

#ifdef _DEBUG
#include <gtest/gtest.h>
#endif

#include "Repository/JsonRepository.h"
#include "Model/SampleModel.h"
#include "Model/OrderModel.h"
#include "Model/ProductionModel.h"
#include "View/SampleView.h"
#include "View/OrderView.h"
#include "View/ProductionView.h"
#include "View/ShipmentView.h"
#include "View/MonitorView.h"
#include "View/MenuView.h"
#include "Controller/SampleController.h"
#include "Controller/OrderController.h"
#include "Controller/ProductionController.h"
#include "Controller/ShipmentController.h"
#include "Controller/MonitorController.h"
#include "Controller/AppController.h"

int main(int argc, char** argv) {
#ifdef _DEBUG
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
    SetConsoleOutputCP(65001);

    JsonRepository<Sample>        sampleRepo("data/samples.json");
    JsonRepository<Order>         orderRepo("data/orders.json");
    JsonRepository<ProductionJob> prodRepo("data/jobs.json");

    SampleModel     sampleModel(sampleRepo);
    OrderModel      orderModel(orderRepo);
    ProductionModel productionModel(prodRepo);

    SampleView     sampleView;
    OrderView      orderView;
    ProductionView productionView;
    ShipmentView   shipmentView;
    MonitorView    monitorView;
    MenuView       menuView;

    SampleController     sampleCtrl(sampleModel, sampleView);
    OrderController      orderCtrl(orderModel, sampleModel, productionModel, orderView);
    ProductionController prodCtrl(productionModel, orderModel, sampleModel, productionView);
    ShipmentController   shipCtrl(orderModel, shipmentView);
    MonitorController    monitorCtrl(orderModel, sampleModel, monitorView);

    AppController app(sampleCtrl, orderCtrl, prodCtrl, shipCtrl, monitorCtrl, menuView);
    app.run();
    return 0;
#endif
}
