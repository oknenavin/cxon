// JSON object as a map

#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/map.hxx"

int main() {
    std::map<std::string, std::vector<double>> o;
        auto r = cxon::from_bytes(o, R"({"k":[42]})");
   return !(r && o.size() == 1 && o.find("k") != o.end() && o["k"].size() == 1 && o["k"][0] == 42);
}
