#include "cxon/json.hxx"
#include "cxon/lib/node.hxx"

int main() {
    using node = cxon::json::node;
     node n;
        auto r = cxon::from_bytes(n, "[42]");
    return !(r && n.is<node::array>() && n.get<node::array>().size() == 1 && n.get<node::array>()[0] == 42);
}
