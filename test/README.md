## `CXON` & `CXJSON` tests


--------------------------------------------------------------------------------
#### `CXON`

All tests are located in `cxon.*.cxx`. A group of tests for given `Traits`
can be defined as this:

``` c++
TEST_BEG(Traits)
    ...
TEST_END()
```

For trivial cases, `R_TEST` and `W_TEST` macros can be used:

``` c++
TEST_BEG(cxon::JSON<>)
    // test bool type
    R_TEST(true, "true");                               // R_TEST(expected-value, string)
    R_TEST(false, "t", read_error::boolean_invalid, 0); //  + expected-error, expected-error-offset
    W_TEST("true", true);                               // W_TEST(string, input-value)
TEST_END()
```

for non-trivial cases, free code can be used, provided that:
- `suite::all()` shall be incremented for each new test
- `suite::err()` shall be incremented if a test fails
- in case of failure, error message + `assert` shall be used to point out failure's location

example:

``` c++
TEST_BEG(cxon::JSON<>)
    ++suite::all();
    int r; char const i[] = "1";
    if (!cxon::from_bytes(r, std::begin(i), std::end(i)) || r != 1) {
        ++suite::err(), fprintf(stderr, "\tat %s:%li\n", __FILE__, (long)__LINE__);
        CXON_ASSERT(false, "check failed");
    }
TEST_END()
```


--------------------------------------------------------------------------------
#### `CXJSON`

Is a utility that accepts command and a list of inputs (or a `@file`, see below):

    cxjson ((pass|fail|diff) (file|@file)+)*

- `pass`  - the input is a list of files that must be read successfully
- `fail`  - the input is a list of files that must fail to read
- `diff`  - for each input file, a pair of files is produced, such that the first file
             is just the input file pretty-printed, and the second one is the result of a
             read/write/pretty-print sequence. These files shall be further diffed
- `file`  - arbitrary valid or invalid `JSON`. Must be relative to the current directory
- `@file` - is a new-line separated list of files. The files must be relative to the
             current directory. Lines starting with `#` are ignored

If no parameters are provided, self tests will be performed.  
For example of its usage, see `test-cxjson` rule in the [makefile](makefile).

###### Suites

Suite                            | Type | Origin
---------------------------------|------|--------------------------------------------------------------------------------------------------
[set.1-pass](data/set.1-pass.in) | pass | [JSON_checker](http://json.org/JSON_checker)
[set.1-fail](data/set.1-fail.in) | fail | [JSON_checker](http://json.org/JSON_checker)
[set.2-pass](data/set.2-pass.in) | pass | [nst/JSONTestSuite](https://github.com/nst/JSONTestSuite)
[set.2-fail](data/set.2-fail.in) | fail | [nst/JSONTestSuite](https://github.com/nst/JSONTestSuite)
[set.3-diff](data/set.3-diff.in) | diff | [minimaxir/big-list-of-naughty-strings](https://github.com/minimaxir/big-list-of-naughty-strings)
[set.4-diff](data/set.4-diff.in) | diff | [github/gemoji](https://github.com/github/gemoji)
[set.5-diff](data/set.5-diff.in) | diff | [lemire/simdjson](https://github.com/lemire/simdjson)

Note that some of the inputs are commented out, because of different reasons - for example:
- [data/set.1/fail1.json](data/set.1/fail1.json) - ignored because `CXON/CXJSON` allows arbitrary
  top-level value
- [data/set.1/fail18.json](data/set.1/fail18.json) - ignored because `CXON/CXJSON` default recursive
  depth is 64
- [data/set.5/gsoc-2018.json](data/set.5/gsoc-2018.json) - ignored because the input contains redundant
  escapes, and the output is normalized
- [data/set.5/marine_ik.json](data/set.5/marine_ik.json) - ignored because the input contains numbers
  with lower precision, and the output numbers are with the highest precision


--------------------------------------------------------------------------------
Distributed under the MIT license. See [`LICENSE`](../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  
