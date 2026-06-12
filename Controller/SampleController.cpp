#include "SampleController.h"
#include <algorithm>
#include <cctype>

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
    if (s.yield < 0.0 || s.yield > 1.0) {
        view_.showMessage("수율은 0.0 ~ 1.0 사이여야 합니다.");
        return;
    }
    for (const auto& existing : model_.getAll())
        if (existing.name == s.name) {
            view_.showMessage("이미 등록된 시료 이름입니다.");
            return;
        }
    model_.add(s);
    view_.showMessage("시료가 등록되었습니다.");
}

void SampleController::handleListAll() {
    view_.showSampleList(model_.getAll());
}

void SampleController::handleSearch() {
    std::string kw = view_.promptSearchKeyword();
    std::vector<Sample> results;
    bool isId = !kw.empty() &&
                std::all_of(kw.begin(), kw.end(), [](unsigned char c){ return std::isdigit(c); });
    if (isId) {
        auto found = model_.findById(std::stoll(kw));
        if (found) results.push_back(*found);
    } else {
        results = model_.searchByName(kw);
    }
    if (results.empty())
        view_.showMessage("검색 결과가 없습니다.");
    else
        view_.showSampleList(results);
}
