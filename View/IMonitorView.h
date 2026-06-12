#pragma once
#include <vector>
#include "../Model/Order.h"
#include "../Model/Sample.h"

class IMonitorView {
public:
    virtual ~IMonitorView() = default;
    virtual void render(const std::vector<Order>& orders,
                        const std::vector<Sample>& samples) = 0;
    virtual void waitKeyPress() = 0;
};
