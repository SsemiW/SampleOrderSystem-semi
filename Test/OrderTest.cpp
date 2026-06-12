#include <gtest/gtest.h>
#include <filesystem>
#include "Test/Mocks.h"
#include "Model/OrderModel.h"
#include "Repository/JsonRepository.h"
#include "Controller/OrderController.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SaveArg;

namespace fs = std::filesystem;

// ─── OrderModel 테스트 (실제 JsonRepository 사용) ─────────────────────────

class OrderModelTest : public ::testing::Test {
protected:
    fs::path              tmpFile_;
    JsonRepository<Order> repo_{ "" };

    void SetUp() override {
        tmpFile_ = fs::temp_directory_path() / "ordermodel_test.json";
        fs::remove(tmpFile_);
        new (&repo_) JsonRepository<Order>(tmpFile_);
    }
    void TearDown() override { fs::remove(tmpFile_); }

    Order make(int64_t sampleId, int qty, OrderStatus status = OrderStatus::Reserved) {
        Order o;
        o.sampleId     = sampleId;
        o.customerName = "테스트고객";
        o.quantity     = qty;
        o.status       = status;
        o.createdAt    = "2026-06-12 09:00:00";
        return o;
    }
};

TEST_F(OrderModelTest, Add_FindById) {
    OrderModel model(repo_);
    int64_t id = model.add(make(1, 30));

    auto found = model.findById(id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->sampleId, 1);
    EXPECT_EQ(found->quantity, 30);
    EXPECT_EQ(found->status, OrderStatus::Reserved);
}

TEST_F(OrderModelTest, GetByStatus) {
    OrderModel model(repo_);
    model.add(make(1, 10, OrderStatus::Reserved));
    model.add(make(2, 20, OrderStatus::Confirmed));
    model.add(make(3, 30, OrderStatus::Reserved));

    auto reserved = model.getByStatus(OrderStatus::Reserved);
    ASSERT_EQ(reserved.size(), 2u);
    for (const auto& o : reserved)
        EXPECT_EQ(o.status, OrderStatus::Reserved);

    auto confirmed = model.getByStatus(OrderStatus::Confirmed);
    ASSERT_EQ(confirmed.size(), 1u);
}

TEST_F(OrderModelTest, UpdateStatus) {
    OrderModel model(repo_);
    int64_t id = model.add(make(1, 30));
    EXPECT_EQ(model.findById(id)->status, OrderStatus::Reserved);

    EXPECT_TRUE(model.updateStatus(id, OrderStatus::Confirmed));
    EXPECT_EQ(model.findById(id)->status, OrderStatus::Confirmed);
}

// ─── OrderController 테스트 (Mock 사용) ──────────────────────────────────

class OrderControllerTest : public ::testing::Test {
protected:
    MockOrderModel      orderModel;
    MockSampleModel     sampleModel;
    MockProductionModel prodModel;
    MockOrderView       view;

    Order makeReserved(int64_t id = 1, int64_t sampleId = 1, int qty = 30) {
        Order o;
        o.id           = id;
        o.sampleId     = sampleId;
        o.customerName = "삼성전자";
        o.quantity     = qty;
        o.status       = OrderStatus::Reserved;
        o.createdAt    = "2026-06-12 09:00:00";
        return o;
    }

    Sample makeSample(int64_t id = 1, int stock = 50, double yield = 0.85) {
        Sample s;
        s.id                = id;
        s.name              = "알파-GaN";
        s.avgProductionTime = 30.0;
        s.yield             = yield;
        s.stock             = stock;
        return s;
    }
};

TEST_F(OrderControllerTest, Reserve_InvalidSample) {
    // 잘못된 시료 ID → 에러 메시지 후 재시도 → 올바른 ID로 성공
    Order bad;  bad.sampleId  = 999; bad.customerName = "Test"; bad.quantity = 10;
    Order good; good.sampleId = 1;   good.customerName = "Test"; good.quantity = 10;

    EXPECT_CALL(view, promptNewOrder())
        .WillOnce(Return(bad))
        .WillOnce(Return(good));
    EXPECT_CALL(sampleModel, findById(999L)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(sampleModel, findById(1L)).WillOnce(Return(makeSample(1)));
    EXPECT_CALL(view, showMessage("존재하지 않는 시료 ID입니다.")).Times(1);
    EXPECT_CALL(orderModel, add(_)).Times(1);
    EXPECT_CALL(view, showMessage("주문이 접수되었습니다.")).Times(1);

    OrderController ctrl(orderModel, sampleModel, prodModel, view);
    ctrl.runReserve();
}

TEST_F(OrderControllerTest, Approve_StockSufficient) {
    Order  reserved  = makeReserved(1, 1, 30);
    Sample sample    = makeSample(1, 50);        // stock(50) >= qty(30)
    Order  confirmed = reserved;
    confirmed.status = OrderStatus::Confirmed;

    EXPECT_CALL(orderModel, getByStatus(OrderStatus::Reserved))
        .WillOnce(Return(std::vector<Order>{reserved}))
        .WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(view, showOrderList(_)).Times(2);
    EXPECT_CALL(view, showOrderMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(view, promptOrderId()).WillOnce(Return(1L));
    EXPECT_CALL(orderModel, findById(1L))
        .WillOnce(Return(reserved))
        .WillOnce(Return(confirmed));
    EXPECT_CALL(sampleModel, findById(1L)).WillOnce(Return(sample));
    EXPECT_CALL(orderModel, updateStatus(1L, OrderStatus::Confirmed)).Times(1);
    EXPECT_CALL(sampleModel, updateStock(1L, -30)).Times(1);
    EXPECT_CALL(view, showApprovalResult(confirmed, _)).Times(1);

    OrderController ctrl(orderModel, sampleModel, prodModel, view);
    ctrl.runApproval();
}

TEST_F(OrderControllerTest, Approve_StockInsufficient) {
    Order  reserved  = makeReserved(1, 1, 30);
    Sample sample    = makeSample(1, 10);        // stock(10) < qty(30)
    Order  producing = reserved;
    producing.status = OrderStatus::Producing;

    EXPECT_CALL(orderModel, getByStatus(OrderStatus::Reserved))
        .WillOnce(Return(std::vector<Order>{reserved}))
        .WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(view, showOrderList(_)).Times(2);
    EXPECT_CALL(view, showOrderMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(view, promptOrderId()).WillOnce(Return(1L));
    EXPECT_CALL(orderModel, findById(1L))
        .WillOnce(Return(reserved))
        .WillOnce(Return(producing));
    EXPECT_CALL(sampleModel, findById(1L)).WillOnce(Return(sample));
    EXPECT_CALL(prodModel,   enqueue(_)).Times(1);
    EXPECT_CALL(orderModel,  updateStatus(1L, OrderStatus::Producing)).Times(1);
    EXPECT_CALL(view, showApprovalResult(_, _)).Times(1);

    OrderController ctrl(orderModel, sampleModel, prodModel, view);
    ctrl.runApproval();
}

TEST_F(OrderControllerTest, Approve_CalcRequiredAmount) {
    // shortage = 30 - 10 = 20, yield = 0.85
    // required = ceil(20 / (0.85 * 0.9)) = ceil(26.14) = 27
    Order  reserved  = makeReserved(1, 1, 30);
    Sample sample    = makeSample(1, 10, 0.85);
    Order  producing = reserved;
    producing.status = OrderStatus::Producing;

    EXPECT_CALL(orderModel, getByStatus(OrderStatus::Reserved))
        .WillOnce(Return(std::vector<Order>{reserved}))
        .WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(view, showOrderList(_)).Times(2);
    EXPECT_CALL(view, showOrderMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(view, promptOrderId()).WillOnce(Return(1L));
    EXPECT_CALL(orderModel, findById(1L))
        .WillOnce(Return(reserved))
        .WillOnce(Return(producing));
    EXPECT_CALL(sampleModel, findById(1L)).WillOnce(Return(sample));
    EXPECT_CALL(orderModel, updateStatus(1L, OrderStatus::Producing)).Times(1);
    EXPECT_CALL(view, showApprovalResult(_, _)).Times(1);

    ProductionJob captured;
    EXPECT_CALL(prodModel, enqueue(_)).WillOnce(SaveArg<0>(&captured));

    OrderController ctrl(orderModel, sampleModel, prodModel, view);
    ctrl.runApproval();

    EXPECT_EQ(captured.requiredAmount, 27);
    EXPECT_EQ(captured.orderId,  1);
    EXPECT_EQ(captured.sampleId, 1);
    EXPECT_DOUBLE_EQ(captured.totalProdTimeMin, 30.0 * 27);
}

TEST_F(OrderControllerTest, Reject) {
    Order reserved = makeReserved(1, 1, 30);

    EXPECT_CALL(orderModel, getByStatus(OrderStatus::Reserved))
        .WillOnce(Return(std::vector<Order>{reserved}))
        .WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(view, showOrderList(_)).Times(2);
    EXPECT_CALL(view, showOrderMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(2))
        .WillOnce(Return(0));
    EXPECT_CALL(view, promptOrderId()).WillOnce(Return(1L));
    EXPECT_CALL(orderModel, findById(1L)).WillOnce(Return(reserved));
    EXPECT_CALL(orderModel, remove(1L)).Times(1);
    EXPECT_CALL(view, showMessage("주문이 거절되었습니다.")).Times(1);

    OrderController ctrl(orderModel, sampleModel, prodModel, view);
    ctrl.runApproval();
}
