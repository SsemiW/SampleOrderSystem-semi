#pragma once
#include "IMenuView.h"

class MenuView : public IMenuView {
public:
    int  selectRole()                        override;
    int  getMenuChoice()                     override;
    void showCustomerMenu()                  override;
    void showOrderManagerMenu()              override;
    void showProductionManagerMenu()         override;
    void showMessage(const std::string& msg) override;
};
