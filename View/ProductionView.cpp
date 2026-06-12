#include "ProductionView.h"
#include <iostream>
#include <iomanip>
#include <string>

void ProductionView::render(const std::optional<ProductionJob>& currentJob,
                            int producedAmount,
                            const std::vector<ProductionJob>& waitingQueue) {
    std::cout << "\n=== 생산라인 현황 ===\n";

    if (currentJob) {
        const auto& j = *currentJob;
        int total = j.requiredAmount;
        int filled = (total > 0) ? (producedAmount * 20 / total) : 0;

        std::cout << "\n[현재 생산 중]\n"
                  << "  주문 #" << j.orderId
                  << "  |  실생산량: " << total << "개"
                  << "  |  총생산시간: " << j.totalProdTimeMin << "분\n"
                  << "  진행: ["
                  << std::string(filled, '#')
                  << std::string(20 - filled, '.')
                  << "] " << producedAmount << "/" << total << "개\n";
    } else {
        std::cout << "\n  현재 생산 중인 작업이 없습니다.\n";
    }

    std::cout << "\n[대기 큐]\n";
    if (waitingQueue.empty()) {
        std::cout << "  대기 중인 작업이 없습니다.\n";
    } else {
        std::cout << "  " << std::left
                  << std::setw(6)  << "순번"
                  << std::setw(10) << "주문#"
                  << std::setw(10) << "실생산량"
                  << std::setw(12) << "총생산시간(분)"
                  << "\n  " << std::string(38, '-') << "\n";
        for (const auto& j : waitingQueue) {
            std::cout << "  "
                      << std::setw(6)  << j.queuePosition
                      << std::setw(10) << j.orderId
                      << std::setw(10) << j.requiredAmount
                      << std::setw(12) << j.totalProdTimeMin
                      << "\n";
        }
    }

    std::cout << "\n[C] 완료 처리  [0] 돌아가기\n> ";

    char input;
    std::cin >> input;
    std::cin.ignore();
    completeRequested_ = (input == 'C' || input == 'c');
}

bool ProductionView::isCompleteRequested() const {
    return completeRequested_;
}
