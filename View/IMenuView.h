#pragma once
#include <string>

class IMenuView {
public:
    virtual ~IMenuView() = default;
    virtual int  selectRole()                        = 0;
    virtual int  getMenuChoice()                     = 0;
    virtual void showCustomerMenu()                  = 0;
    virtual void showOrderManagerMenu()              = 0;
    virtual void showProductionManagerMenu()         = 0;
    virtual void showMessage(const std::string& msg) = 0;
};
