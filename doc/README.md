## `CXON` library

[![Library][img-lib]](https://github.com/oknenavin/cxon)
[![Version][img-ver]](https://github.com/oknenavin/cxon/releases)  


--------------------------------------------------------------------------------

#### Contents
  - [`CXON` core](../src/cxon/README.md)
    - [Introduction](../src/cxon/README.md#introduction)
      - [Supported types](../src/cxon/README.md#supported-types)
    - [Interface](../src/cxon/README.md#interface)
      - [`cxon::from_bytes`](../src/cxon/README.md#read-interface)
      - [`cxon::to_bytes`](../src/cxon/README.md#write-interface)
    - [Implementation bridge](../src/cxon/README.md#implementation-bridge)
    - [Parametrization](../src/cxon/README.md#parametrization)
      - [Format traits](../src/cxon/README.md#format-traits)
      - [Named parameters](../src/cxon/README.md#named-parameters)
      - [Context](../src/cxon/README.md#context)
  - [`CXON/JSON`](../src/cxon/lang/json/README.md)
    - [Introduction](../src/cxon/lang/json/README.md#introduction)
    - [Errors](../src/cxon/lang/json/README.md#errors)
    - [Parameters](../src/cxon/lang/json/README.md#parameters)
    - [Helpers](../src/cxon/lang/json/README.md#helpers)
    - [`cxon::json::basic_node`](../src/cxon/lang/json/node/README.md#basic_node)
    - [`CXON` Integration](../src/cxon/lang/json/node/README.md#cxon-integration)
  - [`CXON/CBOR`](../src/cxon/lang/cbor/README.md)
    - [Introduction](../src/cxon/lang/cbor/README.md#introduction)
    - [Errors](../src/cxon/lang/cbor/README.md#errors)
    - [Parameters](../src/cxon/lang/cbor/README.md#parameters)
    - [Helpers](../src/cxon/lang/cbor/README.md#helpers)
    - [`cxon::cbor::basic_node`](../src/cxon/lang/cbor/node/README.md#basic_node)
    - [`CXON` Integration](../src/cxon/lang/cbor/node/README.md#cxon-integration)
  - [`CXON/CXCF` configuration format](../src/cxon/lang/cxcf/README.md)
  - [Examples](../examples/README.md)


--------------------------------------------------------------------------------

Distributed under the terms of the GNU Affero General Public License. See [`LICENSE`](../LICENSE) for more information.  
[GitHub](https://github.com/oknenavin/cxon)  


<!-- links -->
[img-lib]: https://img.shields.io/badge/lib-CXON-608060.svg?style=plastic
[img-ver]: https://img.shields.io/github/release/oknenavin/cxon.svg?style=plastic&color=608060
