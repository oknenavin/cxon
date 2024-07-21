#include "boost/json.hpp"

int main() {
    boost::system::error_code ec;
    boost::json::parse_options opt;
        opt.numbers = boost::json::number_precision::precise;
    boost::json::value v = boost::json::parse("[42]", ec, {}, opt);
    return !(ec == std::errc{} && v.is_array() && v.as_array().size() == 1 && v.as_array()[0] == 42);
}
