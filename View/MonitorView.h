#pragma once
#include "IMonitorView.h"

class MonitorView : public IMonitorView {
public:
    void render(const std::vector<Order>& orders,
                const std::vector<Sample>& samples) override;
    void waitKeyPress() override;
};
