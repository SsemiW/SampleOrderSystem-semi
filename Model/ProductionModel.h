#pragma once
#include "IProductionModel.h"
#include "../Repository/IRepository.h"
#include <ctime>

class ProductionModel : public IProductionModel {
public:
    explicit ProductionModel(IRepository<ProductionJob>& repo);

    void                         enqueue(ProductionJob job)      override;
    std::optional<ProductionJob> currentJob()             const  override;
    std::vector<ProductionJob>   waitingQueue()           const  override;
    int                          currentProducedAmount()  const  override;
    bool                         isCurrentComplete()      const  override;
    ProductionJob                dequeue()                       override;

private:
    IRepository<ProductionJob>& repo_;

    static std::string  nowString();
    static std::time_t  parseTime(const std::string& s);
    static double       elapsedMinutes(const std::string& startedAt);
};
