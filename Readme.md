# C++26 json-rpc

This is the start of a C++26 json-rpc project.

I've done a lot of heavy lifting here already. We have a client and server, requests,
responses and errors. I have some tests and am working on pushing the system to see
where it breaks.

# Usage

## Server

 1. Write a function:

```
int add(int x, int y) {
  return x + y;
}
```

 2. Create a server:

```
fr::json_rpc::Server<^^Add> addServer;
```

Servers take JSON string requests which you can generate from a client.

```
fr::json_rpc::Client<^^Add> addClient;
// The first parameter to makeRequest is an ID, which can be a string, an int or nullptr.
std::string addRequest = addClient.makeRequest(std::string("addTest"), 2, 2);

// Retrieve the JSON Result from the server
// Currently we have to tell it the type of the request ID

std::string addResponse = addServer.request<std::string>(addRequest);
std::cout << addRequest << std::endl << addResponse << std::endl;
```

That's pretty much it. Take a look at the unit tests (Esp. ServerTest.cpp)
for a full example.

## Todos:

Oh tons of stuff

 * Protocols -- stdin/stdout request response for starters (Working toward a MCP server)
 * Things cereal doesn't know about (like const char * strings) break the thing right now.
 * Haven't tested with complex structs as parameters, but that should work.
 * More tests
 * Actually useful examples
 * CMake package integration

## Building

Any c++26 compiler should do (citation needed lol). I'm using the one built with
my [gcc playground project](https://github.com/FlyingRhenquest/gcc_playground)

So I just go:

```
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/local/gcc/toolchain.cmake
make
```

Unit test results:

```

Running main() from ./googletest/src/gtest_main.cc
[==========] Running 3 tests from 3 test suites.
[----------] Global test environment set-up.
[----------] 1 test from ClientTest
[ RUN      ] ClientTest.BasicTest
{
    "jsonrpc": "2.0",
    "method": "add",
    "params": {
        "x": 4,
        "y": 4
    },
    "id": "SomeId"
}
[       OK ] ClientTest.BasicTest (0 ms)
[----------] 1 test from ClientTest (0 ms total)

[----------] 1 test from ServerTest
[ RUN      ] ServerTest.Basic
{
    "jsonrpc": "2.0",
    "result": 4,
    "id": "addId"
}
{
    "jsonrpc": "2.0",
    "result": 0,
    "id": 4
}
{
    "jsonrpc": "2.0",
    "error": {
        "code": -32601,
        "message": "Method \"Steve\" not found"
    },
    "id": null
}
{
    "jsonrpc": "2.0",
    "result": "Hello, Worlt",
    "id": "helloTest"
}
[       OK ] ServerTest.Basic (0 ms)
[----------] 1 test from ServerTest (0 ms total)

[----------] 1 test from ParamsTest
[ RUN      ] ParamsTest.Basic
[       OK ] ParamsTest.Basic (0 ms)
[----------] 1 test from ParamsTest (0 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 3 tests.
```
