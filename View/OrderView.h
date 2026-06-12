#pragma once
#include "IOrderView.h"

class OrderView : public IOrderView {
public:
    void    showOrderMenu()                                                    override;
    int     getMenuChoice()                                                    override;
    void    showOrderList(const std::vector<Order>& orders)                   override;
    Order   promptNewOrder()                                                   override;
    int64_t promptOrderId()                                                    override;
    void    showApprovalResult(const Order& order, const std::string& reason) override;
    void    showMessage(const std::string& msg)                               override;
};
