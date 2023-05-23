// JSON null mapped to std::optional

#include "cxon/json.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include <cassert>

int main() {
#   ifdef CXON_HAS_LIB_STD_OPTIONAL
        std::vector<std::optional<std::string>> opt;
            cxon::from_bytes(opt, R"(["string", "", null])"); // expecting string or null
        assert(opt == (std::vector<std::optional<std::string>>{
            std::make_optional("string"),
            std::make_optional(""),
            std::nullopt,
        }));
#   endif
}
