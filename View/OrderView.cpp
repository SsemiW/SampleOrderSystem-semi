#include "OrderView.h"
#include <iostream>
#include <iomanip>

void OrderView::showOrderMenu() {
    std::cout << "\n=== 주문 승인/거절 ===\n"
              << "1. 승인\n"
              << "2. 거절\n"
              << "0. 돌아가기\n"
              << "> ";
}

int OrderView::getMenuChoice() {
    int choice = 0;
    std::cin >> choice;
    std::cin.ignore();
    return choice;
}

void OrderView::showOrderList(const std::vector<Order>& orders) {
    if (orders.empty()) {
        std::cout << "  (목록 없음)\n";
        return;
    }
    std::cout << "\n"
              << std::left
              << std::setw(6)  << "ID"
              << std::setw(10) << "시료ID"
              << std::setw(16) << "고객명"
              << std::setw(8)  << "주문량"
              << std::setw(12) << "상태"
              << "접수시각\n"
              << std::string(66, '-') << "\n";
    for (const auto& o : orders) {
        std::cout << std::setw(6)  << o.id
                  << std::setw(10) << o.sampleId
                  << std::setw(16) << o.customerName
                  << std::setw(8)  << o.quantity
                  << std::setw(12) << Order::toString(o.status)
                  << o.createdAt   << "\n";
    }
}

Order OrderView::promptNewOrder() {
    Order o;
    std::cout << "\n[주문 접수]\n"
              << "시료 ID: ";
    std::cin >> o.sampleId;
    std::cin.ignore();
    std::cout << "고객명: ";
    std::getline(std::cin, o.customerName);
    std::cout << "주문량: ";
    std::cin >> o.quantity;
    std::cin.ignore();
    return o;
}

int64_t OrderView::promptOrderId() {
    int64_t id = 0;
    std::cout << "주문 ID: ";
    std::cin >> id;
    std::cin.ignore();
    return id;
}

void OrderView::showApprovalResult(const Order& order, const std::string& reason) {
    std::cout << "\n  [결과] 주문 #" << order.id
              << " → " << Order::toString(order.status)
              << " (" << reason << ")\n";
}

void OrderView::showMessage(const std::string& msg) {
    std::cout << msg << "\n";
}
