## `CXON` & `CXJSON` tests


--------------------------------------------------------------------------------
#### `CXON`

All tests are located in [cxon.cxx](cxon.cxx). A group of tests for given `Traits`
are defined as this:

``` c++
TEST_BEG(Traits)
    ...
TEST_END()
```

and for trivial cases, `R_TEST` and `W_TEST` macros can be used:

``` c++
TEST_BEG(cxon::JSON<>)
    // test bool type
    R_TEST(true, "true");                               // R_TEST(expected-value, string)
    R_TEST(false, "t", read_error::boolean_invalid, 0); //  + expected-error, expected-error-offset
    W_TEST("true", true);                               // W_TEST(string, input-value)
TEST_END()
```

for non-trivial cases, free code can be used, provided that:
- `TEST_A` is incremented, indicated that new test is added
- `TEST_F` is incremented if the test fails
- in case of failure, error message + assert shall be used to point out failure's location

example:

``` c++
TEST_BEG(cxon::JSON<>)
    ++TEST_A;
    int r; char const i[] = "1";
    if (!cxon::from_chars(r, std::begin(i), std::end(i)) || r != 1) {
        ++TEST_F, fprintf(stderr, "\tat %s:%li\n", __FILE__, (long)__LINE__);
        CXON_ASSERT(false, "check failed");
    }
TEST_END()
```


--------------------------------------------------------------------------------
#### `CXJSON`

Is a utility that accepts command and a list of `JSON` inputs
(or a `@file`, containing a list of `JSON` inputs):

    cxjson ((pass|fail|diff) (file|@file)+)+

and the commands are:
- `pass` - the input is a list of files that must be read successfully
- `fail` - the input is a list of files that must fail to read
- `diff` - for each input file, a pair of files is produced, such that the first file  
            is just the input file pretty-printed, and the second is the result of  
            read/write/pretty-print sequence. These files shall further be diffed.

An example of its usage, see `test-cxjson` rule in the [makefile](makefile).


--------------------------------------------------------------------------------
Distributed under the MIT license. See [`LICENSE`](../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  
