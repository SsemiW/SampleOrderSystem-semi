#include <gtest/gtest.h>
#include <filesystem>
#include "Test/Mocks.h"
#include "Model/SampleModel.h"
#include "Repository/JsonRepository.h"
#include "Controller/SampleController.h"

using ::testing::Return;
using ::testing::_;

namespace fs = std::filesystem;

class SampleModelTest : public ::testing::Test {
protected:
    fs::path               tmpFile_;
    JsonRepository<Sample> repo_{ "" };

    void SetUp() override {
        tmpFile_ = fs::temp_directory_path() / "samplemodel_test.json";
        fs::remove(tmpFile_);
        new (&repo_) JsonRepository<Sample>(tmpFile_);
    }

    void TearDown() override {
        fs::remove(tmpFile_);
    }

    Sample make(const std::string& name, int stock = 10) {
        Sample s;
        s.name              = name;
        s.avgProductionTime = 30.0;
        s.yield             = 0.85;
        s.stock             = stock;
        return s;
    }
};

TEST_F(SampleModelTest, Add_FindById) {
    SampleModel model(repo_);
    model.add(make("알파-GaN", 50));

    auto all = model.getAll();
    ASSERT_EQ(all.size(), 1u);
    auto found = model.findById(all[0].id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "알파-GaN");
}

TEST_F(SampleModelTest, GetAll) {
    SampleModel model(repo_);
    model.add(make("A"));
    model.add(make("B"));
    model.add(make("C"));
    EXPECT_EQ(model.getAll().size(), 3u);
}

TEST_F(SampleModelTest, SearchByName) {
    SampleModel model(repo_);
    model.add(make("알파-GaN"));
    model.add(make("베타-SiC"));
    model.add(make("알파-InP"));

    auto result = model.searchByName("알파");
    ASSERT_EQ(result.size(), 2u);
    for (const auto& s : result)
        EXPECT_NE(s.name.find("알파"), std::string::npos);
}

TEST_F(SampleModelTest, UpdateStock_Increase) {
    SampleModel model(repo_);
    model.add(make("A", 10));
    int64_t id = model.getAll()[0].id;

    EXPECT_TRUE(model.updateStock(id, 5));
    EXPECT_EQ(model.findById(id)->stock, 15);
}

TEST_F(SampleModelTest, UpdateStock_Decrease) {
    SampleModel model(repo_);
    model.add(make("A", 10));
    int64_t id = model.getAll()[0].id;

    EXPECT_TRUE(model.updateStock(id, -3));
    EXPECT_EQ(model.findById(id)->stock, 7);
}

TEST_F(SampleModelTest, UpdateStock_NoBelowZero) {
    SampleModel model(repo_);
    model.add(make("A", 5));
    int64_t id = model.getAll()[0].id;

    EXPECT_FALSE(model.updateStock(id, -10));
    EXPECT_EQ(model.findById(id)->stock, 5);
}

TEST(SampleControllerTest, Register) {
    MockSampleModel model;
    MockSampleView  view;

    Sample newSample;
    newSample.name = "테스트시료";

    EXPECT_CALL(view, showSampleMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(view, promptNewSample()).WillOnce(Return(newSample));
    EXPECT_CALL(model, add(_)).Times(1);
    EXPECT_CALL(view, showMessage("시료가 등록되었습니다.")).Times(1);

    SampleController ctrl(model, view);
    ctrl.run();
}

TEST(SampleControllerTest, Search_NoResult) {
    MockSampleModel model;
    MockSampleView  view;

    EXPECT_CALL(view, showSampleMenu()).Times(2);
    EXPECT_CALL(view, getMenuChoice())
        .WillOnce(Return(3))
        .WillOnce(Return(0));
    EXPECT_CALL(view, promptSearchKeyword()).WillOnce(Return("없는이름"));
    EXPECT_CALL(model, searchByName("없는이름"))
        .WillOnce(Return(std::vector<Sample>{}));
    EXPECT_CALL(view, showMessage("검색 결과가 없습니다.")).Times(1);

    SampleController ctrl(model, view);
    ctrl.run();
}
