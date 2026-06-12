#include "SampleController.h"

SampleController::SampleController(ISampleModel& model, ISampleView& view)
    : model_(model), view_(view) {}

void SampleController::run() {
    while (true) {
        view_.showSampleMenu();
        switch (view_.getMenuChoice()) {
        case 1: handleRegister(); break;
        case 2: handleListAll();  break;
        case 3: handleSearch();   break;
        case 0: return;
        }
    }
}

void SampleController::handleRegister() {
    Sample s = view_.promptNewSample();
    model_.add(s);
    view_.showMessage("시료가 등록되었습니다.");
}

void SampleController::handleListAll() {
    view_.showSampleList(model_.getAll());
}

void SampleController::handleSearch() {
    std::string kw = view_.promptSearchKeyword();
    auto results = model_.searchByName(kw);
    if (results.empty())
        view_.showMessage("검색 결과가 없습니다.");
    else
        view_.showSampleList(results);
}
