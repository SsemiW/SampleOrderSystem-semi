#include <gtest/gtest.h>
#include <filesystem>
#include "Model/Sample.h"
#include "Model/Order.h"
#include "Model/ProductionJob.h"
#include "Repository/JsonRepository.h"

namespace fs = std::filesystem;

// ─── Sample JSON RoundTrip ────────────────────────────────────────────────────

TEST(Sample_JsonRoundTrip, AllFields) {
    Sample s;
    s.id                = 1;
    s.name              = "알파-GaN";
    s.avgProductionTime = 30.0;
    s.yield             = 0.85;
    s.stock             = 50;

    EXPECT_EQ(s, Sample::fromJson(s.toJson()));
}

// ─── Order JSON RoundTrip ─────────────────────────────────────────────────────

TEST(Order_JsonRoundTrip, AllFields) {
    Order o;
    o.id           = 2;
    o.sampleId     = 1;
    o.customerName = "삼성전자";
    o.quantity     = 30;
    o.status       = OrderStatus::Confirmed;
    o.createdAt    = "2026-06-12 09:00:00";

    EXPECT_EQ(o, Order::fromJson(o.toJson()));
}

TEST(Order_JsonRoundTrip, AllStatuses) {
    const OrderStatus statuses[] = {
        OrderStatus::Reserved, OrderStatus::Rejected,
        OrderStatus::Producing, OrderStatus::Confirmed, OrderStatus::Release
    };
    for (auto s : statuses)
        EXPECT_EQ(Order::fromString(Order::toString(s)), s);
}

// ─── ProductionJob JSON RoundTrip ─────────────────────────────────────────────

TEST(ProductionJob_JsonRoundTrip, AllFields) {
    ProductionJob j;
    j.id               = 3;
    j.orderId          = 2;
    j.sampleId         = 1;
    j.requiredAmount   = 12;
    j.avgProdTimeMin   = 30.0;
    j.totalProdTimeMin = 360.0;
    j.startedAt        = "2026-06-12 10:30:00";
    j.queuePosition    = 0;

    EXPECT_EQ(j, ProductionJob::fromJson(j.toJson()));
}

TEST(ProductionJob_JsonRoundTrip, EmptyStartedAt) {
    ProductionJob j;
    j.id               = 4;
    j.orderId          = 3;
    j.sampleId         = 1;
    j.requiredAmount   = 5;
    j.avgProdTimeMin   = 20.0;
    j.totalProdTimeMin = 100.0;
    j.startedAt        = "";
    j.queuePosition    = 1;

    EXPECT_EQ(j, ProductionJob::fromJson(j.toJson()));
}

// ─── JsonRepository ───────────────────────────────────────────────────────────

class JsonRepositoryTest : public ::testing::Test {
protected:
    fs::path tmpFile_;

    void SetUp() override {
        tmpFile_ = fs::temp_directory_path() / "sampleorder_repo_test.json";
        fs::remove(tmpFile_);
    }

    void TearDown() override {
        fs::remove(tmpFile_);
    }

    Sample makeSample(const std::string& name, double apt, double yield, int stock) {
        Sample s;
        s.name              = name;
        s.avgProductionTime = apt;
        s.yield             = yield;
        s.stock             = stock;
        return s;
    }
};

TEST_F(JsonRepositoryTest, Create_FindById) {
    JsonRepository<Sample> repo(tmpFile_);
    int64_t id = repo.create(makeSample("알파-GaN", 30.0, 0.85, 50));

    auto found = repo.findById(id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->id, id);
    EXPECT_EQ(found->name, "알파-GaN");
    EXPECT_EQ(found->stock, 50);
}

TEST_F(JsonRepositoryTest, Update_FindById) {
    JsonRepository<Sample> repo(tmpFile_);
    int64_t id = repo.create(makeSample("베타-SiC", 20.0, 0.90, 30));

    auto s = *repo.findById(id);
    s.stock = 100;
    repo.update(s);

    EXPECT_EQ(repo.findById(id)->stock, 100);
}

TEST_F(JsonRepositoryTest, Remove_FindById_Nullopt) {
    JsonRepository<Sample> repo(tmpFile_);
    int64_t id = repo.create(makeSample("감마-InP", 15.0, 0.80, 20));
    repo.remove(id);

    EXPECT_FALSE(repo.findById(id).has_value());
}

TEST_F(JsonRepositoryTest, Persistence_ReloadFromFile) {
    int64_t id;
    {
        JsonRepository<Sample> repo(tmpFile_);
        id = repo.create(makeSample("델타-GaAs", 25.0, 0.75, 10));
    }

    JsonRepository<Sample> repo2(tmpFile_);
    auto found = repo2.findById(id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "델타-GaAs");
    EXPECT_DOUBLE_EQ(found->yield, 0.75);
    EXPECT_EQ(found->stock, 10);
}
