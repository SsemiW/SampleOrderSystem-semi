#pragma once
#include <optional>
#include <vector>
#include <cstdint>

template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual int64_t          create(T item)                  = 0;
    virtual std::optional<T> findById(int64_t id)      const = 0;
    virtual std::vector<T>   findAll()                 const = 0;
    virtual void             update(const T& item)           = 0;
    virtual void             remove(int64_t id)              = 0;
};
