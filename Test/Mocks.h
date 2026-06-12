#pragma once
#include <gmock/gmock.h>
#include "Model/ISampleModel.h"
#include "View/ISampleView.h"

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
