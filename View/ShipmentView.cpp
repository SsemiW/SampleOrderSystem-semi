#include "ShipmentView.h"
#include <iostream>
#include <iomanip>

void ShipmentView::showConfirmedOrders(const std::vector<Order>& orders) {
    std::cout << "\n=== 출고 대기 목록 (CONFIRMED) ===\n";
    if (orders.empty()) {
        std::cout << "  (목록 없음)\n";
        return;
    }
    std::cout << std::left
              << std::setw(6)  << "ID"
              << std::setw(10) << "시료ID"
              << std::setw(16) << "고객명"
              << std::setw(8)  << "주문량"
              << "접수시각\n"
              << std::string(56, '-') << "\n";
    for (const auto& o : orders) {
        std::cout << std::setw(6)  << o.id
                  << std::setw(10) << o.sampleId
                  << std::setw(16) << o.customerName
                  << std::setw(8)  << o.quantity
                  << o.createdAt   << "\n";
    }
}

int64_t ShipmentView::promptOrderId() {
    int64_t id = 0;
    std::cout << "\n출고 처리할 주문 ID: ";
    std::cin >> id;
    std::cin.ignore();
    return id;
}

void ShipmentView::showReleaseResult(const Order& order) {
    std::cout << "\n  [출고 완료] 주문 #" << order.id
              << " → " << Order::toString(order.status) << "\n";
}

void ShipmentView::showMessage(const std::string& msg) {
    std::cout << msg << "\n";
}
