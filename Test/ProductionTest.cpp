#include <gtest/gtest.h>
#include <filesystem>
#include "Test/Mocks.h"
#include "Model/ProductionModel.h"
#include "Repository/JsonRepository.h"
#include "Controller/ProductionController.h"

using ::testing::_;
using ::testing::Return;

namespace fs = std::filesystem;

// ─── ProductionModel 테스트 (실제 JsonRepository 사용) ────────────────────

class ProductionModelTest : public ::testing::Test {
protected:
    fs::path                      tmpFile_;
    JsonRepository<ProductionJob> repo_{ "" };

    void SetUp() override {
        tmpFile_ = fs::temp_directory_path() / "productionmodel_test.json";
        fs::remove(tmpFile_);
        new (&repo_) JsonRepository<ProductionJob>(tmpFile_);
    }
    void TearDown() override { fs::remove(tmpFile_); }

    ProductionJob make(int64_t orderId, int qty, double avgTime = 30.0) {
        ProductionJob j;
        j.orderId          = orderId;
        j.sampleId         = 1;
        j.requiredAmount   = qty;
        j.avgProdTimeMin   = avgTime;
        j.totalProdTimeMin = avgTime * qty;
        j.startedAt        = "";
        return j;
    }
};

TEST_F(ProductionModelTest, Enqueue_FirstJob_SetsPositionZeroAndStartedAt) {
    ProductionModel model(repo_);
    model.enqueue(make(1, 10));

    auto job = model.currentJob();
    ASSERT_TRUE(job.has_value());
    EXPECT_EQ(job->queuePosition, 0);
    EXPECT_FALSE(job->startedAt.empty());
}

TEST_F(ProductionModelTest, Enqueue_SecondJob_SetsPositionOneAndNoStartedAt) {
    ProductionModel model(repo_);
    model.enqueue(make(1, 10));
    model.enqueue(make(2, 20));

    auto waiting = model.waitingQueue();
    ASSERT_EQ(waiting.size(), 1u);
    EXPECT_EQ(waiting[0].queuePosition, 1);
    EXPECT_TRUE(waiting[0].startedAt.empty());
}

TEST_F(ProductionModelTest, WaitingQueue_ReturnsSortedByPosition) {
    ProductionModel model(repo_);
    model.enqueue(make(1, 10));
    model.enqueue(make(2, 20));
    model.enqueue(make(3, 30));

    auto waiting = model.waitingQueue();
    ASSERT_EQ(waiting.size(), 2u);
    EXPECT_EQ(waiting[0].queuePosition, 1);
    EXPECT_EQ(waiting[1].queuePosition, 2);
}

TEST_F(ProductionModelTest, CurrentProducedAmount_FarPastStartedAt_ReturnsCapped) {
    ProductionModel model(repo_);
    model.enqueue(make(1, 5, 1.0));

    auto all = repo_.findAll();
    all[0].startedAt = "2000-01-01 00:00:00";
    repo_.update(all[0]);

    EXPECT_EQ(model.currentProducedAmount(), 5);
}

TEST_F(ProductionModelTest, CurrentProducedAmount_NoJob_ReturnsZero) {
    ProductionModel model(repo_);
    EXPECT_EQ(model.currentProducedAmount(), 0);
}

TEST_F(ProductionModelTest, IsCurrentComplete_TrueWhenFarPastStartedAt) {
    ProductionModel model(repo_);
    model.enqueue(make(1, 1, 1.0));

    auto all = repo_.findAll();
    all[0].startedAt = "2000-01-01 00:00:00";
    repo_.update(all[0]);

    EXPECT_TRUE(model.isCurrentComplete());
}

TEST_F(ProductionModelTest, IsCurrentComplete_FalseWhenNoJob) {
    ProductionModel model(repo_);
    EXPECT_FALSE(model.isCurrentComplete());
}

TEST_F(ProductionModelTest, Dequeue_RemovesHeadAndPromotesWaiter) {
    ProductionModel model(repo_);
    model.enqueue(make(1, 5));
    model.enqueue(make(2, 10));

    auto result = model.dequeue();
    EXPECT_EQ(result.orderId, 1);

    auto newHead = model.currentJob();
    ASSERT_TRUE(newHead.has_value());
    EXPECT_EQ(newHead->queuePosition, 0);
    EXPECT_EQ(newHead->orderId, 2);
    EXPECT_FALSE(newHead->startedAt.empty());
    EXPECT_TRUE(model.waitingQueue().empty());
}

// ─── ProductionController 테스트 (Mock 사용) ─────────────────────────────

class ProductionControllerTest : public ::testing::Test {
protected:
    MockProductionModel prodModel;
    MockOrderModel      orderModel;
    MockSampleModel     sampleModel;
    MockProductionView  view;

    ProductionJob makeJob(int64_t orderId = 1, int64_t sampleId = 1, int qty = 10) {
        ProductionJob j;
        j.id               = 1;
        j.orderId          = orderId;
        j.sampleId         = sampleId;
        j.requiredAmount   = qty;
        j.avgProdTimeMin   = 30.0;
        j.totalProdTimeMin = 300.0;
        j.startedAt        = "2000-01-01 00:00:00";
        j.queuePosition    = 0;
        return j;
    }
};

TEST_F(ProductionControllerTest, Run_CompleteRequestedAndComplete_CallsDequeueAndUpdates) {
    ProductionJob job = makeJob(2, 1, 30);

    EXPECT_CALL(prodModel,  currentJob())            .WillOnce(Return(std::optional<ProductionJob>{job}));
    EXPECT_CALL(prodModel,  currentProducedAmount()) .WillOnce(Return(30));
    EXPECT_CALL(prodModel,  waitingQueue())           .WillOnce(Return(std::vector<ProductionJob>{}));
    EXPECT_CALL(view,       render(_, 30, _));
    EXPECT_CALL(view,       isCompleteRequested())   .WillOnce(Return(true));
    EXPECT_CALL(prodModel,  isCurrentComplete())     .WillOnce(Return(true));
    EXPECT_CALL(prodModel,  dequeue())               .WillOnce(Return(job));
    EXPECT_CALL(sampleModel, updateStock(1L, 30))    .Times(1);
    EXPECT_CALL(orderModel,  updateStatus(2L, OrderStatus::Confirmed)).Times(1);

    ProductionController ctrl(prodModel, orderModel, sampleModel, view);
    ctrl.run();
}

TEST_F(ProductionControllerTest, Run_CompleteRequestedButNotComplete_SkipsDequeue) {
    ProductionJob job = makeJob();

    EXPECT_CALL(prodModel,  currentJob())            .WillOnce(Return(std::optional<ProductionJob>{job}));
    EXPECT_CALL(prodModel,  currentProducedAmount()) .WillOnce(Return(5));
    EXPECT_CALL(prodModel,  waitingQueue())           .WillOnce(Return(std::vector<ProductionJob>{}));
    EXPECT_CALL(view,       render(_, 5, _));
    EXPECT_CALL(view,       isCompleteRequested())   .WillOnce(Return(true));
    EXPECT_CALL(prodModel,  isCurrentComplete())     .WillOnce(Return(false));
    EXPECT_CALL(prodModel,  dequeue()).Times(0);

    ProductionController ctrl(prodModel, orderModel, sampleModel, view);
    ctrl.run();
}

TEST_F(ProductionControllerTest, Run_NotCompleteRequested_SkipsDequeue) {
    EXPECT_CALL(prodModel,  currentJob())            .WillOnce(Return(std::nullopt));
    EXPECT_CALL(prodModel,  currentProducedAmount()) .WillOnce(Return(0));
    EXPECT_CALL(prodModel,  waitingQueue())           .WillOnce(Return(std::vector<ProductionJob>{}));
    EXPECT_CALL(view,       render(_, 0, _));
    EXPECT_CALL(view,       isCompleteRequested())   .WillOnce(Return(false));
    EXPECT_CALL(prodModel,  dequeue()).Times(0);

    ProductionController ctrl(prodModel, orderModel, sampleModel, view);
    ctrl.run();
}
