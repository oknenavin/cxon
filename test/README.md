## `CXON` tests


--------------------------------------------------------------------------------

#### `CXON`

The tests are located in `cxon/test`. A group of tests for given `Traits` 
can be defined like this:

``` c++
TEST_BEG(<identifier>, <traits>, <category>)
    ...
TEST_END()
```

For trivial cases, the `R_TEST` and `W_TEST` macros can be used:

``` c++
TEST_BEG(a_test, cxon::JSON<>, "/core")
    R_TEST(true, "true");                               // R_TEST(expected-value, string)
    R_TEST(false, "t", read_error::boolean_invalid, 0); // ... + expected-error[, expected-error-offset]
    W_TEST("true", true);                               // W_TEST(string, input-value[, expected-error])
TEST_END()
```

For more complex cases, arbitrary code can be used provided that:
- `suite::info::count("<category>")` is incremented for each new test
- `suite::info::errors("<category>")` is incremented for each failed test
- in case of failure, an error message + `assert` shall be used to point out failure's location

###### Example

``` c++
TEST_BEG(a_test, cxon::JSON<>, "/a-category")
    ++suite::info::count(category);
    int ot; char const in[] = "1";
    if (!cxon::from_bytes(ot, in) || ot != 1) {
        ++suite::info::errors(category);
        CXON_ASSERT(false, "check failed");
    }
TEST_END()
```


--------------------------------------------------------------------------------

#### `CXON/JSON/NODE`

Is a utility that accepts a test configuration and optionally a list of tests to be executed:

``` bash
    cxon-json-node tests.cf <test-name>*
```

The test configuration is in [`CXCF`](../src/cxon/lang/cxcf/README.md) format with following content:

```
tests = {
    <test-name> = {
        label       = "<test-kind>"
        group       = ["<set-name>"+]
    }
}
sets = {
    <set-name> = {
        label       = "<set description>"
        group       = ["<file-name>"+]
    }
}
```

See [`JSON`](data/json/tests.cf) and [`CBOR`](data/cbor/tests.cf) configuration of `CXON`'s test suites 
for real example.  
Currently, these test-kinds are implemented:

kind        | description
------------|---------------------------------------------------------------------------------------------------------
pass        | the input is a list of files that must be read successfully
fail        | the input is a list of files that must fail to read
diff        | a pair of files is produced for each input file - prety-print and prety-printed serialization round-trip
time-node   | the serialization times for each input are measured (using cxon/json/node)
time-native | same as above,  but with native types (so, the types must be implemented in code)

If no parameters are provided, the self tests will be executed.  
For example of its usage, see `check-*` rules in the [makefile](makefile).

###### Suites

Suite                                 | Type | Origin
--------------------------------------|------|--------------------------------------------------------------------------------------------------
[set.1-pass](data/json/set.1-pass.in) | pass | [JSON_checker](http://json.org/JSON_checker)
[set.1-fail](data/json/set.1-fail.in) | fail | [JSON_checker](http://json.org/JSON_checker)
[set.2-pass](data/json/set.2-pass.in) | pass | [nst/JSONTestSuite](https://github.com/nst/JSONTestSuite)
[set.2-fail](data/json/set.2-fail.in) | fail | [nst/JSONTestSuite](https://github.com/nst/JSONTestSuite)
[set.3-diff](data/json/set.3-diff.in) | diff | [minimaxir/big-list-of-naughty-strings](https://github.com/minimaxir/big-list-of-naughty-strings)
[set.4-diff](data/json/set.4-diff.in) | diff | [github/gemoji](https://github.com/github/gemoji)
[set.5-diff](data/json/set.5-diff.in) | diff | [lemire/simdjson](https://github.com/lemire/simdjson)

Note that some of the inputs are commented out, because of different reasons, for example:
- [data/set.1/fail1.json](data/set.1/fail1.json) - is ignored because `cxon::json::node` 
  allows arbitrary top-level value
- [data/set.1/fail18.json](data/set.1/fail18.json) - is ignored because `cxon::json::node` 
  default recursive depth is 64
- [data/set.5/gsoc-2018.json](data/set.5/gsoc-2018.json) - is ignored because the input contains 
  redundant escapes, and the output is normalized
- [data/set.5/marine_ik.json](data/set.5/marine_ik.json) - is ignored because the input contains 
  numbers with lower precision, and the output numbers are with the highest precision by default


--------------------------------------------------------------------------------

#### `CXON/CBOR/NODE`

Same as for `JSON`.


--------------------------------------------------------------------------------

Distributed under the MIT license. See [`LICENSE`](../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  
