#pragma once
#include <string>
#include <cstdint>
#include "../Core/JsonValue.h"

struct Sample {
    int64_t     id{ 0 };
    std::string name;
    double      avgProductionTime{ 0.0 };   // 분
    double      yield{ 0.0 };               // 0.0 ~ 1.0
    int         stock{ 0 };

    static Sample fromJson(const Core::JsonValue& v) {
        Sample s;
        s.id                = v.at("id").asInteger();
        s.name              = v.at("name").asString();
        s.avgProductionTime = v.at("avgProductionTime").asDouble();
        s.yield             = v.at("yield").asDouble();
        s.stock             = static_cast<int>(v.at("stock").asInteger());
        return s;
    }

    Core::JsonValue toJson() const {
        Core::JsonValue::Object obj;
        obj["id"]               = id;
        obj["name"]             = name;
        obj["avgProductionTime"]= avgProductionTime;
        obj["yield"]            = yield;
        obj["stock"]            = static_cast<int64_t>(stock);
        return Core::JsonValue(obj);
    }

    bool operator==(const Sample& o) const {
        return id == o.id && name == o.name &&
               avgProductionTime == o.avgProductionTime &&
               yield == o.yield && stock == o.stock;
    }
};
