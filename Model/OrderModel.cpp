#include "OrderModel.h"

OrderModel::OrderModel(IRepository<Order>& repo) : repo_(repo) {}

int64_t OrderModel::add(Order o) {
    return repo_.create(std::move(o));
}

std::optional<Order> OrderModel::findById(int64_t id) const {
    return repo_.findById(id);
}

std::vector<Order> OrderModel::getAll() const {
    return repo_.findAll();
}

std::vector<Order> OrderModel::getByStatus(OrderStatus s) const {
    auto all = repo_.findAll();
    std::vector<Order> result;
    for (const auto& o : all)
        if (o.status == s)
            result.push_back(o);
    return result;
}

bool OrderModel::updateStatus(int64_t id, OrderStatus s) {
    auto opt = repo_.findById(id);
    if (!opt) return false;
    opt->status = s;
    repo_.update(*opt);
    return true;
}
