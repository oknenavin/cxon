#include "nlohmann/json.hpp"

int main () {
    nlohmann::json j;
    try {
        j = nlohmann::json::parse("[42]");
    }
    catch (...) {
        return -1;
    }
    return !(j.is_array() && j.array().size() == 42 && j.array()[0] == 42);
}
