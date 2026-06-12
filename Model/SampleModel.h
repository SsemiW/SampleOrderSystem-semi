#pragma once
#include "ISampleModel.h"
#include "../Repository/IRepository.h"

class SampleModel : public ISampleModel {
public:
    explicit SampleModel(IRepository<Sample>& repo);

    void                  add(Sample s)                              override;
    std::optional<Sample> findById(int64_t id)                 const override;
    std::vector<Sample>   getAll()                             const override;
    std::vector<Sample>   searchByName(const std::string& kw)  const override;
    bool                  updateStock(int64_t id, int delta)         override;

private:
    IRepository<Sample>& repo_;
};
