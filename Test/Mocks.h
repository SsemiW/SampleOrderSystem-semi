#pragma once
#include <gmock/gmock.h>
#include "Model/ISampleModel.h"
#include "Model/IOrderModel.h"
#include "Model/IProductionModel.h"
#include "View/ISampleView.h"
#include "View/IOrderView.h"
#include "View/IShipmentView.h"
#include "View/IProductionView.h"
#include "View/IMonitorView.h"
#include "View/IMenuView.h"
#include "Controller/ISampleController.h"
#include "Controller/IOrderController.h"
#include "Controller/IProductionController.h"
#include "Controller/IShipmentController.h"
#include "Controller/IMonitorController.h"

class MockSampleModel : public ISampleModel {
public:
    MOCK_METHOD(void,                  add,          (Sample s),                        (override));
    MOCK_METHOD(std::optional<Sample>, findById,     (int64_t id),                      (const, override));
    MOCK_METHOD(std::vector<Sample>,   getAll,       (),                                (const, override));
    MOCK_METHOD(std::vector<Sample>,   searchByName, (const std::string& kw),           (const, override));
    MOCK_METHOD(bool,                  updateStock,  (int64_t id, int delta),            (override));
};

class MockSampleView : public ISampleView {
public:
    MOCK_METHOD(void,        showSampleMenu, (),                                    (override));
    MOCK_METHOD(int,         getMenuChoice,  (),                                    (override));
    MOCK_METHOD(void,        showSampleList, (const std::vector<Sample>& samples),  (override));
    MOCK_METHOD(Sample,      promptNewSample,(),                                    (override));
    MOCK_METHOD(std::string, promptSearchKeyword, (),                               (override));
    MOCK_METHOD(void,        showMessage,    (const std::string& msg),              (override));
};

class MockOrderModel : public IOrderModel {
public:
    MOCK_METHOD(int64_t,              add,          (Order o),                             (override));
    MOCK_METHOD(std::optional<Order>, findById,     (int64_t id),                          (const, override));
    MOCK_METHOD(std::vector<Order>,   getAll,       (),                                    (const, override));
    MOCK_METHOD(std::vector<Order>,   getByStatus,  (OrderStatus s),                       (const, override));
    MOCK_METHOD(bool,                 updateStatus, (int64_t id, OrderStatus s),            (override));
};

class MockOrderView : public IOrderView {
public:
    MOCK_METHOD(void,    showOrderMenu,      (),                                                    (override));
    MOCK_METHOD(int,     getMenuChoice,      (),                                                    (override));
    MOCK_METHOD(void,    showOrderList,      (const std::vector<Order>& orders),                   (override));
    MOCK_METHOD(Order,   promptNewOrder,     (),                                                    (override));
    MOCK_METHOD(int64_t, promptOrderId,      (),                                                    (override));
    MOCK_METHOD(void,    showApprovalResult, (const Order& order, const std::string& reason),      (override));
    MOCK_METHOD(void,    showMessage,        (const std::string& msg),                             (override));
};

class MockShipmentView : public IShipmentView {
public:
    MOCK_METHOD(void,    showConfirmedOrders, (const std::vector<Order>& orders), (override));
    MOCK_METHOD(int64_t, promptOrderId,       (),                                 (override));
    MOCK_METHOD(void,    showReleaseResult,   (const Order& order),               (override));
    MOCK_METHOD(void,    showMessage,         (const std::string& msg),           (override));
};

class MockMonitorView : public IMonitorView {
public:
    MOCK_METHOD(void, render,       (const std::vector<Order>& orders, const std::vector<Sample>& samples), (override));
    MOCK_METHOD(void, waitKeyPress, (),                                                                     (override));
};

class MockProductionModel : public IProductionModel {
public:
    MOCK_METHOD(void,                         enqueue,               (ProductionJob job),  (override));
    MOCK_METHOD(std::optional<ProductionJob>, currentJob,            (),                   (const, override));
    MOCK_METHOD(std::vector<ProductionJob>,   waitingQueue,          (),                   (const, override));
    MOCK_METHOD(int,                          currentProducedAmount, (),                   (const, override));
    MOCK_METHOD(bool,                         isCurrentComplete,     (),                   (const, override));
    MOCK_METHOD(ProductionJob,                dequeue,               (),                   (override));
};

class MockProductionView : public IProductionView {
public:
    MOCK_METHOD(void, render,               (const std::optional<ProductionJob>&, int, const std::vector<ProductionJob>&), (override));
    MOCK_METHOD(bool, isCompleteRequested,  (),                                                                             (const, override));
};

class MockMenuView : public IMenuView {
public:
    MOCK_METHOD(int,  selectRole,               (),                       (override));
    MOCK_METHOD(int,  getMenuChoice,            (),                       (override));
    MOCK_METHOD(void, showCustomerMenu,         (),                       (override));
    MOCK_METHOD(void, showOrderManagerMenu,     (),                       (override));
    MOCK_METHOD(void, showProductionManagerMenu,(),                       (override));
    MOCK_METHOD(void, showMessage,              (const std::string& msg), (override));
};

class MockSampleController : public ISampleController {
public:
    MOCK_METHOD(void, run, (), (override));
};

class MockOrderController : public IOrderController {
public:
    MOCK_METHOD(void, runReserve,  (), (override));
    MOCK_METHOD(void, runApproval, (), (override));
};

class MockProductionController : public IProductionController {
public:
    MOCK_METHOD(void, run, (), (override));
};

class MockShipmentController : public IShipmentController {
public:
    MOCK_METHOD(void, run, (), (override));
};

class MockMonitorController : public IMonitorController {
public:
    MOCK_METHOD(void, run, (), (override));
};
