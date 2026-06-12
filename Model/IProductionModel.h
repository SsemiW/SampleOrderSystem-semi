#pragma once
#include <vector>
#include <optional>
#include "ProductionJob.h"

class IProductionModel {
public:
    virtual ~IProductionModel() = default;
    virtual void                         enqueue(ProductionJob job)       = 0;
    virtual std::optional<ProductionJob> currentJob()             const   = 0;
    virtual std::vector<ProductionJob>   waitingQueue()           const   = 0;
    virtual int                          currentProducedAmount()  const   = 0;
    virtual bool                         isCurrentComplete()      const   = 0;
    virtual ProductionJob                dequeue()                        = 0;
};
