#include "SampleView.h"
#include <iostream>
#include <iomanip>

void SampleView::showSampleMenu() {
    std::cout << "\n=== 시료 관리 ===\n"
              << "1. 시료 등록\n"
              << "2. 전체 조회\n"
              << "3. 이름 검색\n"
              << "0. 돌아가기\n"
              << "> ";
}

int SampleView::getMenuChoice() {
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    return choice;
}

void SampleView::showSampleList(const std::vector<Sample>& samples) {
    if (samples.empty()) {
        std::cout << "등록된 시료가 없습니다.\n";
        return;
    }
    std::cout << "\n"
              << std::left
              << std::setw(6)  << "ID"
              << std::setw(16) << "이름"
              << std::setw(14) << "평균생산시간(분)"
              << std::setw(10) << "수율"
              << std::setw(8)  << "재고"
              << "\n"
              << std::string(54, '-') << "\n";
    for (const auto& s : samples) {
        std::cout << std::setw(6)  << s.id
                  << std::setw(16) << s.name
                  << std::setw(14) << s.avgProductionTime
                  << std::setw(10) << s.yield
                  << std::setw(8)  << s.stock
                  << "\n";
    }
}

Sample SampleView::promptNewSample() {
    Sample s;
    std::cout << "이름: ";
    std::getline(std::cin, s.name);
    std::cout << "평균 생산시간(분): ";
    std::cin >> s.avgProductionTime;
    std::cout << "수율(0.0~1.0): ";
    std::cin >> s.yield;
    std::cin.ignore();
    return s;
}

std::string SampleView::promptSearchKeyword() {
    std::string kw;
    std::cout << "검색어: ";
    std::getline(std::cin, kw);
    return kw;
}

void SampleView::showMessage(const std::string& msg) {
    std::cout << msg << "\n";
}
