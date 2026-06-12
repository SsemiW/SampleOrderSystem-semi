#pragma once
#include <string>

struct MenuStats {
    std::string datetime;
    int sampleCount{0};
    int totalStock{0};
    int orderCount{0};
    int productionCount{0};
};

class IMenuView {
public:
    virtual ~IMenuView() = default;
    virtual void showMainMenu(const MenuStats& stats) = 0;
    virtual int  getMenuChoice()                      = 0;
    virtual void showMessage(const std::string& msg)  = 0;
};
