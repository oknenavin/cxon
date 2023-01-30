// common interface

#include "cxon/json.hxx" // included first, JSON will be the default format
#include "cxon/cbor.hxx" // not the first, CBOR format must be explicitly specified
// use for all formats, the relevant code will be included depending of the formats included above
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"

#include <cassert>

int main() {
    using CBOR = cxon::CBOR<>;

    char const  json_in[] = "[1,5,7]";
    std::string json_out;

    std::vector<unsigned char> cbor;
    {
        std::vector<int> cxx;
        auto fbr = cxon::from_bytes(cxx, json_in);      // JSON to C++
            assert(fbr);
        auto tbr = cxon::to_bytes<CBOR>(cbor, cxx);     // C++ to CBOR
            assert(tbr);
    }
    {
        std::vector<int> cxx;
        auto fbr = cxon::from_bytes<CBOR>(cxx, cbor);   // CBOR to C++
            assert(fbr);
        auto tbr = cxon::to_bytes(json_out, cxx);       // C++ to JSON
            assert(tbr);
    }

    assert(json_out == json_in);
}
