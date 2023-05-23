// A toy [`JSON-RPC`](https://www.jsonrpc.org/specification) implementation and
// an example of its usage with `CXON`.

#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/tuple.hxx"
#include <cassert>

namespace jsonrpc {

    // request

    template <typename T>
        struct napa { // named parameter
            char const*const key;
            T const value;

            template <typename X, typename O, typename Cx, typename Y = X>
                auto write_value(O& o, Cx& cx) const -> cxon::enable_for_t<Y, cxon::JSON> {
                    return  cxon::cio::write_map_key<Y>(o, key, cx) &&
                            cxon::cio::write_map_val<Y>(o, value, cx)
                    ;
                }
        };
    template <typename T>
        constexpr napa<T> make_napa(const char* k, T&& v) {
            return {k, std::forward<T>(v)};
        }

    template <typename ...P>
        struct request {
            static char const*const jsonrpc;
            std::size_t const       id;
            char const*const        method;
            std::tuple<P...> const  params;

            constexpr request(std::size_t id, const char* method, P&&... params) noexcept
            :   id(id), method(method), params(std::forward<P>(params)...)
            {
            }

            CXON_JSON_CLS_WRITE_MEMBER(request,
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS_DFLT(id, self.id == std::size_t(-1)),
                CXON_JSON_CLS_FIELD_ASIS(method),
                CXON_JSON_CLS_FIELD_ASIS(params)
            )
        };
    template <typename ...P>
        char const*const request<P...>::jsonrpc = "2.0";

    template <typename ...P>
        constexpr request<P...> make_request(std::size_t id, const char* method, P&&... params) {
            return request<P...>(id, method, std::forward<P>(params)...);
        }

    // response

    template <typename D>
        struct error {
            int         code;
            std::string message;
            D           data;

            CXON_JSON_CLS_READ_MEMBER(error,
                CXON_JSON_CLS_FIELD_ASIS(code),
                CXON_JSON_CLS_FIELD_ASIS(message),
                CXON_JSON_CLS_FIELD_ASIS(data)
            )
        };

    template <typename R, typename D = cxon::cio::val::sink<>>
        struct response {
            char            jsonrpc[8];
            std::size_t     id;
            R               result;
            struct error<D> error;

            constexpr response() noexcept
            :   jsonrpc{0}, id(), result(), error()
            {
            }

            CXON_JSON_CLS_READ_MEMBER(response,
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(result),
                CXON_JSON_CLS_FIELD_ASIS(error)
            )
        };

}

namespace cxon { // json-rpc - serialize tuple of named parameters as a JSON object instead of an array

    template <typename X, typename ...T>
        struct write<JSON<X>, std::tuple<jsonrpc::napa<T>...>> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const std::tuple<jsonrpc::napa<T>...>& t, Cx& cx) {
                    return  cio::poke<Y>(o, Y::map::beg, cx) &&
                                cio::cnt::write_tuple<Y>(o, t, cx) &&
                            cio::poke<Y>(o, Y::map::end, cx)
                    ;
                }
        };

}

int main() {
    using namespace cxon;
    {   // params array
        auto const call = jsonrpc::make_request(1, "sub", 42, 23);
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        assert(w && req == R"({"jsonrpc":"2.0","id":1,"method":"sub","params":[42,23]})");
    }
    {   // params object
        auto const call = jsonrpc::make_request(1, "sub", jsonrpc::make_napa("x", 42), jsonrpc::make_napa("y", 23));
        std::string req; // serialize call to req
            auto const w = to_bytes(req, call);
        assert(w && req == R"({"jsonrpc":"2.0","id":1,"method":"sub","params":{"x":42,"y":23}})");
    }
    {   // round-trip: req -> res success
        char const res[] = R"({"jsonrpc": "2.0", "result": 19, "id": 1})";
        jsonrpc::response<int> ret; // serialize res to ret
            auto const r = from_bytes(ret, res);
        assert(r && ret.id == 1 && ret.result == 19);
    }
    {   // round-trip: req -> res failure
        char const res[] = R"({
            "jsonrpc": "2.0",
            "error": {"code": 42, "message": "divide by zero", "data": "a black hole has been created somewhere"},
            "id": 1
        })";
        {   // serialize res to ret, error's data will be skipped
            jsonrpc::response<int> ret;
                auto const r = from_bytes(ret, res);
            assert( r &&
                    ret.id == 1 &&
                    ret.error.code == 42 &&
                    ret.error.message == "divide by zero"
            );
        }
        {   // serialize res to ret, error's data is bound to std::string
            jsonrpc::response<int, std::string> ret;
                auto const r = from_bytes(ret, res);
            assert( r &&
                    ret.id == 1 &&
                    ret.error.code == 42 &&
                    ret.error.message == "divide by zero" &&
                    ret.error.data == "a black hole has been created somewhere"
            );
        }
    }
}
