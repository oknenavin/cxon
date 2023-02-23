// json heterogeneous arrays as a tuples

#include "cxon/json.hxx"
#include "cxon/lib/std/tuple.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include <cassert>

static char in[] = // some heterogeneous JSON arrays
    R"([["Fibonacci", 5, [0, 1, 1, 2, 3]],)"
    R"( ["Pascal", 5, [1, 8, 24, 32, 16]]])"
;
// mapped to std::tuple
using sequence = std::tuple<std::string, unsigned, std::vector<unsigned>>;

int main() {
    std::vector<sequence> sqs;
        cxon::from_bytes(sqs, std::begin(in), std::end(in));
    assert(sqs == (std::vector<sequence>{
        sequence {"Fibonacci", 5, std::vector<unsigned> {0, 1, 1, 2, 3}},
        sequence {"Pascal", 5, std::vector<unsigned> {1, 8, 24, 32, 16}}
    }));
}
