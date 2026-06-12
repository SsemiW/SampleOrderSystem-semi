#include "MenuView.h"
#include <iostream>

namespace {

int dispWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if      (c >= 0xE0) { w += 2; i += 3; }
        else if (c >= 0xC0) { w += 1; i += 2; }
        else                { w += 1; i += 1; }
    }
    return w;
}

std::string padR(const std::string& s, int width) {
    int d = width - dispWidth(s);
    return d > 0 ? s + std::string(d, ' ') : s;
}

std::string hbar(int n = 46) {
    std::string s;
    for (int i = 0; i < n; ++i) s += "═";
    return s;
}

std::string row(const std::string& content, int innerW = 46) {
    return "║" + padR(content, innerW) + "║\n";
}

} // namespace

void MenuView::showMainMenu(const MenuStats& s) {
    static const std::string H = hbar();

    std::string statsLeft1  = "  등록시료: " + std::to_string(s.sampleCount) + "종";
    std::string statsRight1 = "총재고: "     + std::to_string(s.totalStock)   + "개";
    std::string statsLeft2  = "  전체주문: " + std::to_string(s.orderCount)   + "건";
    std::string statsRight2 = "생산라인: "   + std::to_string(s.productionCount) + "건";

    std::cout
        << "╔" << H << "╗\n"
        << row("    반도체 시료 생산주문관리 시스템")
        << "╠" << H << "╣\n"
        << row("  시스템 현황: " + s.datetime)
        << "╠" << H << "╣\n"
        << row(padR(statsLeft1, 21) + statsRight1)
        << row(padR(statsLeft2, 21) + statsRight2)
        << "╠" << H << "╣\n"
        << row("  1. 시료 관리")
        << row("  2. 주문 등록")
        << row("  3. 주문 승인 / 거절")
        << row("  4. 모니터링")
        << row("  5. 생산라인 조회")
        << row("  6. 출고 처리")
        << row("  0. 종료")
        << "╚" << H << "╝\n"
        << "선택: ";
}

int MenuView::getMenuChoice() {
    int ch;
    std::cin >> ch;
    return ch;
}

void MenuView::showMessage(const std::string& msg) {
    std::cout << msg << '\n';
}
