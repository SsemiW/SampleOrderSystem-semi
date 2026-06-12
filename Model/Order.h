#pragma once
#include <string>
#include <cstdint>
#include <stdexcept>
#include "../Core/JsonValue.h"

enum class OrderStatus {
    Reserved,
    Rejected,
    Producing,
    Confirmed,
    Release
};

struct Order {
    int64_t     id{ 0 };
    int64_t     sampleId{ 0 };
    std::string customerName;
    int         quantity{ 0 };
    OrderStatus status{ OrderStatus::Reserved };
    std::string createdAt;  // "YYYY-MM-DD HH:MM:SS"

    static OrderStatus fromString(const std::string& s) {
        if (s == "RESERVED")  return OrderStatus::Reserved;
        if (s == "REJECTED")  return OrderStatus::Rejected;
        if (s == "PRODUCING") return OrderStatus::Producing;
        if (s == "CONFIRMED") return OrderStatus::Confirmed;
        if (s == "RELEASE")   return OrderStatus::Release;
        throw std::invalid_argument("Unknown OrderStatus: " + s);
    }

    static std::string toString(OrderStatus s) {
        switch (s) {
        case OrderStatus::Reserved:  return "RESERVED";
        case OrderStatus::Rejected:  return "REJECTED";
        case OrderStatus::Producing: return "PRODUCING";
        case OrderStatus::Confirmed: return "CONFIRMED";
        case OrderStatus::Release:   return "RELEASE";
        }
        return "UNKNOWN";
    }

    static Order fromJson(const Core::JsonValue& v) {
        Order o;
        o.id           = v.at("id").asInteger();
        o.sampleId     = v.at("sampleId").asInteger();
        o.customerName = v.at("customerName").asString();
        o.quantity     = static_cast<int>(v.at("quantity").asInteger());
        o.status       = fromString(v.at("status").asString());
        o.createdAt    = v.at("createdAt").asString();
        return o;
    }

    Core::JsonValue toJson() const {
        Core::JsonValue::Object obj;
        obj["id"]           = id;
        obj["sampleId"]     = sampleId;
        obj["customerName"] = customerName;
        obj["quantity"]     = static_cast<int64_t>(quantity);
        obj["status"]       = toString(status);
        obj["createdAt"]    = createdAt;
        return Core::JsonValue(obj);
    }

    bool operator==(const Order& o) const {
        return id == o.id && sampleId == o.sampleId &&
               customerName == o.customerName && quantity == o.quantity &&
               status == o.status && createdAt == o.createdAt;
    }
};
