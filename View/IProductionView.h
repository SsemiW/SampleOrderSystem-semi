#pragma once
#include <vector>
#include <optional>
#include "../Model/ProductionJob.h"

class IProductionView {
public:
    virtual ~IProductionView() = default;
    virtual void render(const std::optional<ProductionJob>& currentJob,
                        int producedAmount,
                        const std::vector<ProductionJob>& waitingQueue) = 0;
    virtual bool isCompleteRequested() const = 0;
};
