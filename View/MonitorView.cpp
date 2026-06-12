#include "MonitorView.h"
#include <iostream>
#include <map>
#include <sstream>

namespace {

// UTF-8 인식 표시 너비 계산 (CJK = 2칸, 그 외 = 1칸)
int dispWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if      (c < 0x80) { w += 1; i += 1; }
        else if (c < 0xE0) { w += 1; i += 2; }
        else if (c < 0xF0) { w += 2; i += 3; }
        else               { w += 2; i += 4; }
    }
    return w;
}

// 오른쪽 공백 패딩 (표시 너비 기준)
std::string padR(const std::string& s, int w) {
    int pad = w - dispWidth(s);
    return s + (pad > 0 ? std::string(pad, ' ') : "");
}

// 수평선 문자 반복
std::string hbar(int n) {
    std::string r;
    r.reserve(static_cast<size_t>(n) * 3);
    for (int i = 0; i < n; ++i) r += "═";
    return r;
}

// 재고 상태 레이블
std::string stockLabel(int stock, int confirmedQty) {
    if (stock == 0)              return "고갈";
    if (stock < confirmedQty)    return "부족";
    return "여유";
}

} // namespace

void MonitorView::render(const std::vector<Order>& orders,
                         const std::vector<Sample>& samples) {
    // 주문 상태 카운트 (REJECTED 제외)
    int nRes = 0, nPro = 0, nCon = 0, nRel = 0;
    for (const auto& o : orders) {
        switch (o.status) {
        case OrderStatus::Reserved:  ++nRes; break;
        case OrderStatus::Producing: ++nPro; break;
        case OrderStatus::Confirmed: ++nCon; break;
        case OrderStatus::Release:   ++nRel; break;
        default: break;
        }
    }

    // 시료별 CONFIRMED 주문량 합계
    std::map<int64_t, int> cQty;
    for (const auto& o : orders)
        if (o.status == OrderStatus::Confirmed)
            cQty[o.sampleId] += o.quantity;

    // 왼쪽·오른쪽 행 구성
    std::vector<std::string> left = {
        "  ■ 주문 현황",
        "  RESERVED  : " + std::to_string(nRes) + "건",
        "  PRODUCING : " + std::to_string(nPro) + "건",
        "  CONFIRMED : " + std::to_string(nCon) + "건",
        "  RELEASE   : " + std::to_string(nRel) + "건"
    };

    std::vector<std::string> right;
    right.push_back("  ■ 시료별 재고 현황");
    for (const auto& s : samples) {
        int cq = cQty.count(s.id) ? cQty.at(s.id) : 0;
        std::ostringstream oss;
        oss << "  " << s.name << "  " << s.stock
            << "   " << stockLabel(s.stock, cq);
        right.push_back(oss.str());
    }

    // 행 수 맞추기
    size_t rows = std::max(left.size(), right.size());
    left.resize(rows);
    right.resize(rows);

    // ─── 박스 출력 ─────────────────────────────────────────
    constexpr int L = 20;   // 왼쪽 섹션 표시 너비
    constexpr int R = 25;   // 오른쪽 섹션 표시 너비

    auto boxRow = [&](const std::string& l, const std::string& r) {
        std::cout << "║" << padR(l, L)
                  << "║" << padR(r, R) << "║\n";
    };

    std::cout << "\n"
              << "╔" << hbar(L + 1 + R) << "╗\n"
              << "║" << padR("  모니터링 — 시스템 현황", L + 1 + R) << "║\n"
              << "╠" << hbar(L) << "╦" << hbar(R) << "╣\n";
    for (size_t i = 0; i < rows; ++i)
        boxRow(left[i], right[i]);
    std::cout << "╚" << hbar(L) << "╩" << hbar(R) << "╝\n";
}

void MonitorView::waitKeyPress() {
    std::cout << "  [Enter] 돌아가기";
    std::cin.ignore();
}
