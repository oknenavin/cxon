// overriding default serializers

#include "cxon/json.hxx"
#include <cassert>

namespace cxon {
    // override the default double parser for JSON<X>
    template <typename X>
        struct read<JSON<X>, double> {
            template <typename II, typename Cx>
                static bool value(double& t, II&, II, Cx&) {
                    // implement fast double parsing for example
                    return t = 42., true;
                }
        };
}

struct custom_traits : cxon::json::format_traits {};
using CUSTOM_JSON = cxon::JSON<custom_traits>;

namespace cxon {
    // override the default float parser for CUSTOM_JSON, leave the default JSON<> as is
    template <>
        struct read<CUSTOM_JSON, float> {
            template <typename II, typename Cx>
                static bool value(float& t, II&, II, Cx&) {
                    // implement custom float parsing
                    return t = 42., true;
                }
        };
}

int main() {
    double d = 0.0;
        cxon::from_bytes(d, "0");
    assert(d == 42.);   // the override is used for double
    int i = 0;
        cxon::from_bytes(i, "42");
    assert(i == 42);    // the other types are intact
    float f = 0.0;
        cxon::from_bytes<CUSTOM_JSON>(f, "0");
    assert(f == 42.);   // the override is used for CUSTOM_JSON
    unsigned u = 0;
        cxon::from_bytes<CUSTOM_JSON>(u, "42");
    assert(u == 42);    // the other types are as in JSON<>
}
