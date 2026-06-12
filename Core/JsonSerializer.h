#pragma once
#include "JsonValue.h"
#include <string>
#include <filesystem>

namespace Core {

class JsonSerializer {
public:
    static std::string serialize(const JsonValue& value, bool pretty = false, int indent = 4);

    static bool saveFile(const JsonValue& value,
                         const std::filesystem::path& filePath,
                         bool pretty = true,
                         int  indent = 4);

private:
    JsonSerializer(bool pretty, int indent) : pretty_(pretty), indent_(indent) {}

    std::string serializeValue (const JsonValue&         v, int depth) const;
    std::string serializeString(const std::string&       v)            const;
    std::string serializeDouble(double                   v)            const;
    std::string serializeArray (const JsonValue::Array&  v, int depth) const;
    std::string serializeObject(const JsonValue::Object& v, int depth) const;
    std::string indent(int depth) const;

    bool pretty_;
    int  indent_;
};

} // namespace Core
