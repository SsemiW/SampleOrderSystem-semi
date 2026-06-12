#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "../Model/Order.h"

class IOrderView {
public:
    virtual ~IOrderView() = default;
    virtual void    showOrderMenu()                                                    = 0;
    virtual int     getMenuChoice()                                                    = 0;
    virtual void    showOrderList(const std::vector<Order>& orders)                   = 0;
    virtual Order   promptNewOrder()                                                   = 0;
    virtual int64_t promptOrderId()                                                    = 0;
    virtual void    showApprovalResult(const Order& order, const std::string& reason) = 0;
    virtual void    showMessage(const std::string& msg)                               = 0;
};
