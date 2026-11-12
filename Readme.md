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
// The first parameter to makeRequest is an ID, which can be a string, an int, a std::nullopt
// or a std::monostate
std::string addRequest = addClient.encode(std::string("addTest"), 2, 2);

// Retrieve the JSON Result from the server

std::string addResponse = addServer.request(addRequest);
std::cout << addRequest << std::endl << addResponse << std::endl;
```

You can also decode the response from the server. Decode directly returns the value the function
returned, or an Error. If an error is returned, Decode will throw the error.

```
  fr::json_rpc::Server<^^ServerTestHello, ^^ServerTestPlusTwo> server;
  fr::json_rpc::Client<^^ServerTestHello> helloClient;
  std::string helloRequest = helloClient.encode(1, "Worlt");
  std::string helloResponse = server.request(helloRequest);
  std::string helloResult = helloClient.decode(helloResponse);
  ASSERT_EQ(helloResult, "Hello, Worlt!");
  ASSERT_EQ(helloClient.lastId(), 1);
```

## Stdexec-based Pipelines

I just wrote the first one of these, a simple in-place pipeline.
`fr::json_rpc::pipelines::InProcess accepts a client and a server, an ID to pass to
the client and the parameters to pass to the client. It returns a stdexec pipeline
that can be scheduled to call the server and the client in the same process.
See the PipelinesTest for an example. It's very basic, but it'll be a while
before I really have my head wrapped around the library. I'll work my way
up to increasingly complex ones as I have time.

## Todos:

Oh tons of stuff

 * Moar Pipelines!
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
