#pragma once
#include "IProductionView.h"

class ProductionView : public IProductionView {
public:
    void render(const std::optional<ProductionJob>& currentJob,
                int producedAmount,
                const std::vector<ProductionJob>& waitingQueue) override;
    bool isCompleteRequested() const override;

private:
    bool completeRequested_{ false };
};
