#pragma once
#include <vector>
#include <string>
#include "../Model/Order.h"

class IShipmentView {
public:
    virtual ~IShipmentView() = default;
    virtual void    showConfirmedOrders(const std::vector<Order>& orders) = 0;
    virtual int64_t promptOrderId()                                        = 0;
    virtual void    showReleaseResult(const Order& order)                  = 0;
    virtual void    showMessage(const std::string& msg)                    = 0;
};
