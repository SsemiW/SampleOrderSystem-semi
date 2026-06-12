#pragma once
#include <string>
#include <cstdint>
#include "../Core/JsonValue.h"

struct ProductionJob {
    int64_t     id{ 0 };                  // 저장소 기본 키 (auto-assign)
    int64_t     orderId{ 0 };
    int64_t     sampleId{ 0 };
    int         requiredAmount{ 0 };      // 실생산량
    double      avgProdTimeMin{ 0.0 };    // 단위당 평균 생산시간 (분)
    double      totalProdTimeMin{ 0.0 };  // 총생산시간 (분)
    std::string startedAt;                // 큐 선두 진입 시각, 대기 중이면 ""
    int         queuePosition{ 0 };       // 0 = 생산 중, 1+ = 대기

    static ProductionJob fromJson(const Core::JsonValue& v) {
        ProductionJob j;
        j.id               = v.at("id").asInteger();
        j.orderId          = v.at("orderId").asInteger();
        j.sampleId         = v.at("sampleId").asInteger();
        j.requiredAmount   = static_cast<int>(v.at("requiredAmount").asInteger());
        j.avgProdTimeMin   = v.at("avgProdTimeMin").asDouble();
        j.totalProdTimeMin = v.at("totalProdTimeMin").asDouble();
        j.startedAt        = v.at("startedAt").asString();
        j.queuePosition    = static_cast<int>(v.at("queuePosition").asInteger());
        return j;
    }

    Core::JsonValue toJson() const {
        Core::JsonValue::Object obj;
        obj["id"]               = id;
        obj["orderId"]          = orderId;
        obj["sampleId"]         = sampleId;
        obj["requiredAmount"]   = static_cast<int64_t>(requiredAmount);
        obj["avgProdTimeMin"]   = avgProdTimeMin;
        obj["totalProdTimeMin"] = totalProdTimeMin;
        obj["startedAt"]        = startedAt;
        obj["queuePosition"]    = static_cast<int64_t>(queuePosition);
        return Core::JsonValue(obj);
    }

    bool operator==(const ProductionJob& o) const {
        return id == o.id && orderId == o.orderId && sampleId == o.sampleId &&
               requiredAmount == o.requiredAmount &&
               avgProdTimeMin == o.avgProdTimeMin &&
               totalProdTimeMin == o.totalProdTimeMin &&
               startedAt == o.startedAt && queuePosition == o.queuePosition;
    }
};
