#include "cxon/json.hxx"
#include "cxon/lib/std/vector.hxx"

int main() {
    std::vector<int> v;
        auto r = cxon::from_bytes(v, "[42]");
   return !(r && v.size() == 1 && v[0] == 42);
}
