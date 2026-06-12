#include "AppController.h"

AppController::AppController(ISampleController&     sample,
                             IOrderController&      order,
                             IProductionController& production,
                             IShipmentController&   shipment,
                             IMonitorController&    monitor,
                             IMenuView&             view)
    : sample_(sample), order_(order), production_(production),
      shipment_(shipment), monitor_(monitor), view_(view) {}

void AppController::run() {
    while (true) {
        switch (view_.selectRole()) {
        case 1: runCustomer();          break;
        case 2: runOrderManager();      break;
        case 3: runProductionManager(); break;
        case 0: return;
        }
    }
}

void AppController::runCustomer() {
    while (true) {
        view_.showCustomerMenu();
        switch (view_.getMenuChoice()) {
        case 1: sample_.run();       break;
        case 2: order_.runReserve(); break;
        case 0: return;
        }
    }
}

void AppController::runOrderManager() {
    while (true) {
        view_.showOrderManagerMenu();
        switch (view_.getMenuChoice()) {
        case 1: order_.runApproval(); break;
        case 2: monitor_.run();       break;
        case 0: return;
        }
    }
}

void AppController::runProductionManager() {
    while (true) {
        view_.showProductionManagerMenu();
        switch (view_.getMenuChoice()) {
        case 1: production_.run(); break;
        case 2: shipment_.run();   break;
        case 3: monitor_.run();    break;
        case 0: return;
        }
    }
}
