#ifndef STRUTL_HXX_
#define STRUTL_HXX_

#include <string>
#include <tuple>
#include <valarray>
#include <array>
#include <queue>
#include <stack>
#include <deque>
#include <list>
#include <forward_list>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <memory>

#include <cstdarg>

#ifndef CXON_ASSERT
#   ifndef _MSC_VER
#       include <cassert>
#       define CXON_ASSERT(e, m) assert(e)
#   else
#       include <crtdbg.h>
#       define CXON_ASSERT(e, m) _ASSERT_EXPR((e), m)
#   endif
#endif

///////////////////////////////////////////////////////////////////////////////

namespace strutl { // to_string

    using namespace std;

    inline string to_string() {
        return {};
    }
    template <typename H, typename ...T> struct str {
        static string it() {
            return str<H>::it() + ", " + str<T...>::it();
        }
    };
    template <typename T> struct str<T> {
        static string it() {
            CXON_ASSERT(0, "unsupported type");
            return "<unknown-type>";
        }
    };

#   define STRUTL_DEF(T) template <> struct str<T> { static string it() { return #T; } }
        STRUTL_DEF(bool);
        STRUTL_DEF(char);
        STRUTL_DEF(wchar_t);
        STRUTL_DEF(char16_t);
        STRUTL_DEF(char32_t);
        STRUTL_DEF(signed char);
        STRUTL_DEF(unsigned char);
        STRUTL_DEF(short);
        STRUTL_DEF(unsigned short);
        STRUTL_DEF(int);
        STRUTL_DEF(unsigned int);
        STRUTL_DEF(long);
        STRUTL_DEF(unsigned long);
        STRUTL_DEF(long long);
        STRUTL_DEF(unsigned long long);
        STRUTL_DEF(float);
        STRUTL_DEF(double);
        STRUTL_DEF(long double);
        STRUTL_DEF(std::nullptr_t);
        STRUTL_DEF(std::string);
        STRUTL_DEF(std::wstring);
        STRUTL_DEF(std::u16string);
        STRUTL_DEF(std::u32string);
#   undef STRUTL_DEF

    template <typename ...T> struct str<tuple<T...>> {
        static string it() {
            return "std::tuple<" + str<T...>::it() + '>';
        }
    };
    template <typename F, typename S> struct str<pair<F, S>> {
        static string it() {
            return "std::pair<" + str<F, S>::it() + '>';
        }
    };
    template <typename T> struct str<valarray<T>> {
        static string it() {
            return "std::valarray<" + str<T>::it() + '>';
        }
    };
    template <typename T, size_t S> struct str<array<T, S>> {
        static string it() {
            return "std::array<" + str<T>::it() + ", " + std::to_string(S) + '>';
        }
    };
    template <typename T, typename C> struct str<queue<T, C>> {
        static string it() {
            return "std::queue<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename C> struct str<priority_queue<T, C>> {
        static string it() {
            return "std::priority_queue<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename C> struct str<stack<T, C>> {
        static string it() {
            return "std::stack<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename A> struct str<deque<T, A>> {
        static string it() {
            return "std::deque<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename A> struct str<list<T, A>> {
        static string it() {
            return "std::list<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename A> struct str<forward_list<T, A>> {
        static string it() {
            return "std::forward_list<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename A> struct str<vector<T, A>> {
        static string it() {
            return "std::vector<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename C, typename A> struct str<set<T, C, A>> {
        static string it() {
            return "std::set<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename C, typename A> struct str<multiset<T, C, A>> {
        static string it() {
            return "std::multiset<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename C, typename A> struct str<unordered_set<T, C, A>> {
        static string it() {
            return "std::unordered_set<" + str<T>::it() + '>';
        }
    };
    template <typename T, typename C, typename A> struct str<unordered_multiset<T, C, A>> {
        static string it() {
            return "std::unordered_multiset<" + str<T>::it() + '>';
        }
    };
    template <typename K, typename V, typename C, typename A> struct str<map<K, V, C, A>> {
        static string it() {
            return "std::map<" + str<K, V>::it() + '>';
        }
    };
    template <typename K, typename V, typename C, typename A> struct str<multimap<K, V, C, A>> {
        static string it() {
            return "std::multimap<" + str<K, V>::it() + '>';
        }
    };
    template <typename K, typename V, typename C, typename A> struct str<unordered_map<K, V, C, A>> {
        static string it() {
            return "std::unordered_map<" + str<K, V>::it() + '>';
        }
    };
    template <typename K, typename V, typename C, typename A> struct str<unordered_multimap<K, V, C, A>> {
        static string it() {
            return "std::unordered_multimap<" + str<K, V>::it() + '>';
        }
    };
    template <typename T> struct str<T*> {
        static string it() {
            return str<typename remove_cv<T>::type>::it() + " *";
        }
    };
    template <typename T, size_t N> struct str<T[N]> {
        static string it() {
            return str<T>::it() + "[" + std::to_string(N) + "]";
        }
    };

    template <typename ...T> inline string to_string(const T&...) {
        return str<T...>::it();
    }
    template <typename ...T> inline string to_string() {
        return str<T...>::it();
    }

}   // strutl to_string

#define STRUTL_TO_STRING_SIMPLE(T) template <> struct strutl::str<T> { static string it() { return #T; } }

#define STRUTL_TO_STRING_STRUCT_BEGIN(T)\
    template <> struct strutl::str<T> {\
        static string it() {\
            string s = "struct " #T "{ "
#define STRUTL_TO_STRING_STRUCT_FIELD(f)\
            s += str<decltype(T::f)>::it() + " " #f "; "
#define STRUTL_TO_STRING_STRUCT_END()\
            s += "}";\
            return s;\
        }\
    }

namespace strutl { // format

    using namespace std;

	inline string formatv(const char* fmt, va_list val) {
	    CXON_ASSERT(fmt, "unexpected format");
	    int const r = vsnprintf(nullptr, 0, fmt, val) + 1;
	    unique_ptr<char[]> s(new char[r]);
	    int const w = vsnprintf(s.get(), r, fmt, val) + 1; CXON_ASSERT(r == w, "write failure");
	    return string(s.get(), w);
	}

    inline string format(const char* fmt, ...) {
	    CXON_ASSERT(fmt, "unexpected format");
	    va_list val; va_start(val, fmt);
	    string s = formatv(fmt, val);
	    va_end(val);
	    return s;
	}

}   // strutl format

#endif // STRUTL_HXX_
