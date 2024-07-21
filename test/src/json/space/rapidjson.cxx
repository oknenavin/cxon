#include "rapidjson/document.h"

int main() {
    rapidjson::Document d;
    auto constexpr opt = rapidjson::kParseValidateEncodingFlag|rapidjson::kParseFullPrecisionFlag;
    rapidjson::ParseResult r = d.Parse<opt>("[42]");
    return !(r && d.IsArray() && d.GetArray().Size() == 1 && d.GetArray()[0] == 42);
}
