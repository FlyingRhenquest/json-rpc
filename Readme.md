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
std::string addRequest = addClient.encode(std::string("addTest"), 2, 2);

// Retrieve the JSON Result from the server
// Currently we have to tell it the type of the request ID

std::string addResponse = addServer.request<std::string>(addRequest);
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

That's pretty much it. Take a look at the unit tests (Esp. ServerTest.cpp)
for a full example.

## Todos:

Oh tons of stuff

 * Transports -- I'm planning to use nvidia's stdexec for this.
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
