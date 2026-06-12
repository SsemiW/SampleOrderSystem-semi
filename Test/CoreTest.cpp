#include <gtest/gtest.h>
#include "Core/JsonParser.h"
#include "Core/JsonSerializer.h"
#include <stdexcept>

using namespace Core;

// ─── 헬퍼: JsonValue 재귀 동등 비교 ─────────────────────────────────────

namespace {

bool jsonEq(const JsonValue& a, const JsonValue& b) {
    if (a.type() != b.type()) return false;
    switch (a.type()) {
        case JsonValue::Type::Null:    return true;
        case JsonValue::Type::Bool:    return a.asBool()    == b.asBool();
        case JsonValue::Type::Integer: return a.asInteger() == b.asInteger();
        case JsonValue::Type::Double:  return a.asDouble()  == b.asDouble();
        case JsonValue::Type::String:  return a.asString()  == b.asString();
        case JsonValue::Type::Array: {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i)
                if (!jsonEq(a[i], b[i])) return false;
            return true;
        }
        case JsonValue::Type::Object: {
            const auto& ao = a.asObject();
            const auto& bo = b.asObject();
            if (ao.size() != bo.size()) return false;
            for (const auto& [k, v] : ao) {
                auto it = bo.find(k);
                if (it == bo.end() || !jsonEq(v, it->second)) return false;
            }
            return true;
        }
    }
    return false;
}
}


TEST(JsonParser_Primitives, Null) {
    auto v = JsonParser::parse("null");
    EXPECT_TRUE(v.isNull());
}

TEST(JsonParser_Primitives, BoolTrue) {
    auto v = JsonParser::parse("true");
    ASSERT_TRUE(v.isBool());
    EXPECT_TRUE(v.asBool());
}

TEST(JsonParser_Primitives, BoolFalse) {
    auto v = JsonParser::parse("false");
    ASSERT_TRUE(v.isBool());
    EXPECT_FALSE(v.asBool());
}

TEST(JsonParser_Primitives, Integer) {
    auto v = JsonParser::parse("42");
    ASSERT_TRUE(v.isInteger());
    EXPECT_EQ(v.asInteger(), 42);
}

TEST(JsonParser_Primitives, NegativeInteger) {
    auto v = JsonParser::parse("-7");
    ASSERT_TRUE(v.isInteger());
    EXPECT_EQ(v.asInteger(), -7);
}

TEST(JsonParser_Primitives, Double) {
    auto v = JsonParser::parse("3.14");
    ASSERT_TRUE(v.isDouble());
    EXPECT_DOUBLE_EQ(v.asDouble(), 3.14);
}

TEST(JsonParser_Primitives, String) {
    auto v = JsonParser::parse("\"hello\"");
    ASSERT_TRUE(v.isString());
    EXPECT_EQ(v.asString(), "hello");
}

TEST(JsonParser_Primitives, StringWithEscapes) {
    auto v = JsonParser::parse("\"line1\\nline2\\ttab\"");
    EXPECT_EQ(v.asString(), "line1\nline2\ttab");
}

TEST(JsonParser_Primitives, InvalidInput_Throws) {
    EXPECT_THROW(JsonParser::parse("???"), std::runtime_error);
}


TEST(JsonParser_Array, Empty) {
    auto v = JsonParser::parse("[]");
    ASSERT_TRUE(v.isArray());
    EXPECT_EQ(v.size(), 0u);
}

TEST(JsonParser_Array, IntArray) {
    auto v = JsonParser::parse("[1, 2, 3]");
    ASSERT_TRUE(v.isArray());
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0].asInteger(), 1);
    EXPECT_EQ(v[1].asInteger(), 2);
    EXPECT_EQ(v[2].asInteger(), 3);
}

TEST(JsonParser_Array, MixedTypes) {
    auto v = JsonParser::parse("[null, true, 1, 2.5, \"hi\"]");
    ASSERT_EQ(v.size(), 5u);
    EXPECT_TRUE(v[0].isNull());
    EXPECT_TRUE(v[1].asBool());
    EXPECT_EQ(v[2].asInteger(), 1);
    EXPECT_DOUBLE_EQ(v[3].asDouble(), 2.5);
    EXPECT_EQ(v[4].asString(), "hi");
}

TEST(JsonParser_Array, TypeMismatch_Throws) {
    auto v = JsonParser::parse("[1, 2]");
    EXPECT_THROW(v[0].asString(), std::bad_variant_access);
}

TEST(JsonParser_Array, Nested) {
    auto v = JsonParser::parse("[[1, 2], [3, 4]]");
    ASSERT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0][1].asInteger(), 2);
    EXPECT_EQ(v[1][0].asInteger(), 3);
}


TEST(JsonParser_Object, Empty) {
    auto v = JsonParser::parse("{}");
    ASSERT_TRUE(v.isObject());
    EXPECT_EQ(v.size(), 0u);
}

TEST(JsonParser_Object, SimpleFields) {
    auto v = JsonParser::parse("{\"name\": \"GaN\", \"stock\": 50}");
    ASSERT_TRUE(v.isObject());
    EXPECT_EQ(v.at("name").asString(), "GaN");
    EXPECT_EQ(v.at("stock").asInteger(), 50);
}

TEST(JsonParser_Object, Nested) {
    auto v = JsonParser::parse("{\"outer\": {\"inner\": 99}}");
    EXPECT_EQ(v.at("outer").at("inner").asInteger(), 99);
}

TEST(JsonParser_Object, MissingKey_Throws) {
    auto v = JsonParser::parse("{\"a\": 1}");
    EXPECT_THROW(v.at("b"), std::out_of_range);
}

TEST(JsonParser_Object, ObjectInArray) {
    auto v = JsonParser::parse("[{\"id\": 1}, {\"id\": 2}]");
    ASSERT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0].at("id").asInteger(), 1);
    EXPECT_EQ(v[1].at("id").asInteger(), 2);
}


TEST(JsonSerializer_RoundTrip, Null) {
    JsonValue v;
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}

TEST(JsonSerializer_RoundTrip, BoolTrue) {
    JsonValue v(true);
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}

TEST(JsonSerializer_RoundTrip, BoolFalse) {
    JsonValue v(false);
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}

TEST(JsonSerializer_RoundTrip, Integer) {
    JsonValue v(int64_t(99999));
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}

TEST(JsonSerializer_RoundTrip, Double) {
    JsonValue v(0.85);
    auto rt = JsonParser::parse(JsonSerializer::serialize(v));
    ASSERT_TRUE(rt.isDouble());
    EXPECT_DOUBLE_EQ(v.asDouble(), rt.asDouble());
}

TEST(JsonSerializer_RoundTrip, String) {
    JsonValue v("hello\nworld\ttab");
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}

TEST(JsonSerializer_RoundTrip, Array) {
    auto v = JsonParser::parse("[1, \"two\", 3.0, true, null]");
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}

TEST(JsonSerializer_RoundTrip, Object) {
    auto v = JsonParser::parse("{\"id\": 1, \"name\": \"GaN\", \"yield\": 0.85}");
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}

TEST(JsonSerializer_RoundTrip, NestedObject) {
    auto v = JsonParser::parse("{\"sample\": {\"id\": 1, \"stock\": 50}}");
    EXPECT_TRUE(jsonEq(v, JsonParser::parse(JsonSerializer::serialize(v))));
}


TEST(JsonSerializer_PrettyPrint, ContainsNewlines) {
    auto v = JsonParser::parse("{\"a\": 1}");
    std::string out = JsonSerializer::serialize(v, /*pretty=*/true);
    EXPECT_NE(out.find('\n'), std::string::npos);
}

TEST(JsonSerializer_PrettyPrint, ContainsIndent4) {
    auto v = JsonParser::parse("{\"a\": 1}");
    std::string out = JsonSerializer::serialize(v, /*pretty=*/true, /*indent=*/4);
    EXPECT_NE(out.find("    "), std::string::npos);
}

TEST(JsonSerializer_PrettyPrint, CompactHasNoNewlines) {
    auto v = JsonParser::parse("{\"a\": 1}");
    std::string out = JsonSerializer::serialize(v, /*pretty=*/false);
    EXPECT_EQ(out.find('\n'), std::string::npos);
}

TEST(JsonSerializer_PrettyPrint, ArrayPretty) {
    auto v = JsonParser::parse("[1, 2, 3]");
    std::string out = JsonSerializer::serialize(v, /*pretty=*/true);
    EXPECT_NE(out.find('\n'), std::string::npos);
}

