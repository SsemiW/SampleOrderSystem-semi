#pragma once
#include <vector>
#include <optional>
#include <string>
#include <cstdint>
#include "Sample.h"

class ISampleModel {
public:
    virtual ~ISampleModel() = default;
    virtual void                  add(Sample s)                              = 0;
    virtual std::optional<Sample> findById(int64_t id)                 const = 0;
    virtual std::vector<Sample>   getAll()                             const = 0;
    virtual std::vector<Sample>   searchByName(const std::string& kw)  const = 0;
    virtual bool                  updateStock(int64_t id, int delta)         = 0;
};
