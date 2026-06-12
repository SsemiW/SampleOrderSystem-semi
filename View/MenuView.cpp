#include "MenuView.h"
#include <iostream>

int MenuView::selectRole() {
    std::cout << "\n╔═══════════════════════╗\n"
              << "║    역할을 선택하세요    ║\n"
              << "╠═══════════════════════╣\n"
              << "║  1. 고객               ║\n"
              << "║  2. 주문 담당자         ║\n"
              << "║  3. 생산 담당자         ║\n"
              << "║  0. 종료               ║\n"
              << "╚═══════════════════════╝\n"
              << "선택: ";
    int ch;
    std::cin >> ch;
    return ch;
}

int MenuView::getMenuChoice() {
    int ch;
    std::cin >> ch;
    return ch;
}

void MenuView::showCustomerMenu() {
    std::cout << "\n[고객 메뉴]\n"
              << "1. 시료 관리\n"
              << "2. 주문 접수\n"
              << "0. 뒤로가기\n"
              << "선택: ";
}

void MenuView::showOrderManagerMenu() {
    std::cout << "\n[주문 담당자 메뉴]\n"
              << "1. 주문 승인/거절\n"
              << "2. 모니터링\n"
              << "0. 뒤로가기\n"
              << "선택: ";
}

void MenuView::showProductionManagerMenu() {
    std::cout << "\n[생산 담당자 메뉴]\n"
              << "1. 생산 라인 운영\n"
              << "2. 출고 처리\n"
              << "3. 모니터링\n"
              << "0. 뒤로가기\n"
              << "선택: ";
}

void MenuView::showMessage(const std::string& msg) {
    std::cout << msg << '\n';
}
