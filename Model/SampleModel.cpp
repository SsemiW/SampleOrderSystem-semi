#include "SampleModel.h"

SampleModel::SampleModel(IRepository<Sample>& repo) : repo_(repo) {}

void SampleModel::add(Sample s) {
    repo_.create(std::move(s));
}

std::optional<Sample> SampleModel::findById(int64_t id) const {
    return repo_.findById(id);
}

std::vector<Sample> SampleModel::getAll() const {
    return repo_.findAll();
}

std::vector<Sample> SampleModel::searchByName(const std::string& kw) const {
    auto all = repo_.findAll();
    std::vector<Sample> result;
    for (const auto& s : all)
        if (s.name.find(kw) != std::string::npos)
            result.push_back(s);
    return result;
}

bool SampleModel::updateStock(int64_t id, int delta) {
    auto s = repo_.findById(id);
    if (!s) return false;
    int newStock = s->stock + delta;
    if (newStock < 0) return false;
    s->stock = newStock;
    repo_.update(*s);
    return true;
}
