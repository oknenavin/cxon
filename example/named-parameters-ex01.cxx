// named parameters

#include "cxon/json.hxx"
#include <cassert>

int main() {
    using namespace cxon::json;
    std::string json;
        cxon::to_bytes(json, 3.1415926, fp_precision::set<3>()); // floating-point precision
    assert(json == "3.14");
}
