#pragma once
#include <vector>
#include <optional>
#include <cstdint>
#include "Order.h"

class IOrderModel {
public:
    virtual ~IOrderModel() = default;
    virtual int64_t              add(Order o)                              = 0;
    virtual std::optional<Order> findById(int64_t id)               const = 0;
    virtual std::vector<Order>   getAll()                            const = 0;
    virtual std::vector<Order>   getByStatus(OrderStatus s)          const = 0;
    virtual bool                 updateStatus(int64_t id, OrderStatus s)  = 0;
};
