#include "boost/json.hpp"

int main() {
    boost::system::error_code ec;
    boost::json::value v = boost::json::parse("[42]", ec);
    return !(ec == std::errc{} && v.is_array() && v.as_array().size() == 1 && v.as_array()[0] == 42);
}
