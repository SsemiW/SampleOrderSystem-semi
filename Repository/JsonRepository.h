#pragma once
// T 요구사항: int64_t id 필드, static T fromJson(const Core::JsonValue&), Core::JsonValue toJson() const
#include <vector>
#include <optional>
#include <algorithm>
#include <filesystem>
#include "IRepository.h"
#include "../Core/JsonValue.h"
#include "../Core/JsonParser.h"
#include "../Core/JsonSerializer.h"

template <typename T>
class JsonRepository : public IRepository<T> {
public:
    explicit JsonRepository(std::filesystem::path filePath)
        : filePath_(std::move(filePath))
    {
        load();
    }

    int64_t create(T item) override {
        item.id = nextId_++;
        items_.push_back(item);
        save();
        return item.id;
    }

    std::optional<T> findById(int64_t id) const override {
        for (const auto& item : items_)
            if (item.id == id) return item;
        return std::nullopt;
    }

    std::vector<T> findAll() const override {
        return items_;
    }

    void update(const T& item) override {
        for (auto& existing : items_) {
            if (existing.id == item.id) {
                existing = item;
                save();
                return;
            }
        }
    }

    void remove(int64_t id) override {
        auto it = std::remove_if(items_.begin(), items_.end(),
                                 [id](const T& x){ return x.id == id; });
        items_.erase(it, items_.end());
        save();
    }

private:
    std::filesystem::path filePath_;
    std::vector<T>        items_;
    int64_t               nextId_{ 1 };

    void load() {
        if (!std::filesystem::exists(filePath_)) return;

        Core::JsonValue root = Core::JsonParser::parseFile(filePath_);
        if (!root.isArray()) return;

        for (const auto& elem : root.asArray()) {
            T item = T::fromJson(elem);
            items_.push_back(item);
            if (item.id >= nextId_) nextId_ = item.id + 1;
        }
    }

    void save() const {
        std::filesystem::create_directories(filePath_.parent_path());
        Core::JsonValue::Array arr;
        arr.reserve(items_.size());
        for (const auto& item : items_)
            arr.push_back(item.toJson());
        Core::JsonSerializer::saveFile(Core::JsonValue(std::move(arr)), filePath_, true);
    }
};
