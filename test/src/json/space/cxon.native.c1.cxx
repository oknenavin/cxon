// JSON object as a struct

#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/list.hxx"

struct my_type {
    std::vector<double> even;
    std::list<double> odd;
    bool operator ==(const my_type& v) const noexcept {
        return even == v.even && odd == v.odd;
    }
};

CXON_JSON_CLS(my_type,
    CXON_JSON_CLS_FIELD_ASIS(even),
    CXON_JSON_CLS_FIELD_ASIS(odd)
)

int main() {
    my_type t;
        auto r = cxon::from_bytes(t, R"({"even": [2, 4, 6], "odd": [1, 3, 5]})");
    return !(r && t == my_type { {2, 4, 6}, {1, 3, 5} });
}
