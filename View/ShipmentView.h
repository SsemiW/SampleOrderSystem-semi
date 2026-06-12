#pragma once
#include "IShipmentView.h"

class ShipmentView : public IShipmentView {
public:
    void    showConfirmedOrders(const std::vector<Order>& orders) override;
    int64_t promptOrderId()                                        override;
    void    showReleaseResult(const Order& order)                  override;
    void    showMessage(const std::string& msg)                    override;
};
