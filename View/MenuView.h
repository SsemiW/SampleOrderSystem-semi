#pragma once
#include "IMenuView.h"

class MenuView : public IMenuView {
public:
    void showMainMenu(const MenuStats& stats) override;
    int  getMenuChoice()                      override;
    void showMessage(const std::string& msg)  override;
};
