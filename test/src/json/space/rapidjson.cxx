#include "rapidjson/document.h"

int main() {
    rapidjson::Document d;
    rapidjson::ParseResult r = d.Parse("[42]");
    return !(r && d.IsArray() && d.GetArray().Size() == 1 && d.GetArray()[0] == 42);
}
