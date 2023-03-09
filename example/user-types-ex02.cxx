#include "cxon/json.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/memory.hxx"
#include <cassert>

// class hierarchy with a base B and derived D1 and D2

struct B {
    enum type { D1, D2 } type;

    B(enum type t) : type(t) {}
    virtual ~B() {}
};
// the base class needs not be serializable

struct D1 : B {
    int d1;

    D1(int n) : B(type::D1), d1(n) {}
};
// derived classes should be writable
CXON_JSON_CLS_WRITE(D1, CXON_JSON_CLS_FIELD_ASIS(d1))

struct D2 : B {
    std::string d2;

    D2(std::string&& s) : B(type::D2), d2(std::move(s)) {}
};
CXON_JSON_CLS_WRITE(D2, CXON_JSON_CLS_FIELD_ASIS(d2))


// class holding a vector of B pointers
struct U {
    std::vector<std::unique_ptr<B>> polymorphic;
};
CXON_JSON_CLS(U, CXON_JSON_CLS_FIELD_ASIS(polymorphic))


namespace cxon {

    // this class will be used to parse B pointers
    // and should be a union of all derived classes
    struct B_union {
        int d1;                     // from D1
        std::string d2;             // from D2
        B_union() : d1(), d2() {}   // define proper defaults as from all the members
                                    // the real type should be decidable
        // tis class is only used for reading
        CXON_JSON_CLS_READ_MEMBER(B_union, CXON_JSON_CLS_FIELD_ASIS(d1), CXON_JSON_CLS_FIELD_ASIS(d2))
    };

    // specialize the read/write structs for B*

    template <typename X>
        struct read<JSON<X>, B*> {
            template <typename II, typename Cx, typename Y = JSON<X>>
                static bool value(B*& t, II& i, II e, Cx& cx) {
                    if (!cio::consume<Y>(i, e, cx)) // consume any white-spaces
                        return false;
                    if (cio::peek(i, e) == *Y::id::nil) // check/read null
                        return read_value<Y>(t, i, e, cx);
                    B_union p; // the union type that represents a B*
                        if (read_value<Y>(p, i, e, cx)) { // try reading it
                            if (p.d1) // let d1 != 0 indicates D1
                                return (t = alc::make_in_context<D1>(cx, p.d1)); // create and assign D1
                            // create and assign D2
                            return (t = alc::make_in_context<D2>(cx, std::move(p.d2)));
                        }
                        return false;
                }
        };

    template <typename X>
        struct write<JSON<X>, B*> {
            template <typename O, typename Cx, typename Y = JSON<X>>
                static bool value(O& o, const B* t, Cx& cx) {
                    if (!t) // check/write null
                            return write_value<Y>(o, nullptr, cx);
                    // cast and write the runtime type
                    switch (t->type) {
                        case B::D1:
                            return write_value<Y>(o, static_cast<const D1*>(t), cx);
                        case B::D2:
                            return write_value<Y>(o, static_cast<const D2*>(t), cx);
                    }
                    return false;
                }
        };

}


int main() {
    U u;
        cxon::from_bytes(u,
            R"({"polymorphic": [{"d2": "42"}, {"d1": 42}]})"
        );
    std::string o;
        cxon::to_bytes(o, u);
    assert(o == R"({"polymorphic":[{"d2":"42"},{"d1":42}]})");
}
