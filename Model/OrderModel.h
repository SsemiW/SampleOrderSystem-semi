#pragma once
#include "IOrderModel.h"
#include "../Repository/IRepository.h"

class OrderModel : public IOrderModel {
public:
    explicit OrderModel(IRepository<Order>& repo);

    int64_t              add(Order o)                              override;
    std::optional<Order> findById(int64_t id)               const override;
    std::vector<Order>   getAll()                            const override;
    std::vector<Order>   getByStatus(OrderStatus s)          const override;
    bool                 updateStatus(int64_t id, OrderStatus s)  override;

private:
    IRepository<Order>& repo_;
};
