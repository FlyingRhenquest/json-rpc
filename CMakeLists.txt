cmake_minimum_required(VERSION 4.2)

project(json-rpc LANGUAGES CXX)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  fixed_string
  GIT_REPOSITORY https://github.com/unterumarmung/fixed_string.git
  GIT_TAG v0.1.1
  FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(fixed_string)

set(REFLECTCPP_USE_STD_EXPECTED ON CACHE INTERNAL "Use std::expected for return values")
set(REFLECTCPP_USE_CPP26_REFLECTION ON CACHE INTERNAL "Use C++26 Reflection")
FetchContent_Declare(
  reflect-cpp
  GIT_REPOSITORY https://github.com/munich-data-engineering/reflect-cpp.git
  GIT_TAG v0.26.2
  FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(reflect-cpp)

set(STDEXEC_BUILD_TESTS OFF CACHE INTERNAL "Disable stdexec tests" FORCE)
set(STDEXEC_BUILD_EXAMPLES OFF CACHE INTERNAL "Disable stdexec tests" FORCE)
FetchContent_Declare(
  stdexec
  GIT_REPOSITORY https://github.com/NVIDIA/stdexec.git
  GIT_TAG main
  FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(stdexec)

set(INTERFACE_HEADERS
  "include/fr/json-rpc/Client.h"
  "include/fr/json-rpc/Error.h"
  "include/fr/json-rpc/Function.h"
  "include/fr/json-rpc/Helpers.h"
  "include/fr/json-rpc/Pipelines.h"
  "include/fr/json-rpc/Request.h"
  "include/fr/json-rpc/Response.h"
  "include/fr/json-rpc/Server.h"
)

add_library(json-rpc INTERFACE)

target_include_directories(json-rpc INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(json-rpc INTERFACE fixed_string::fixed_string reflectcpp::reflectcpp STDEXEC::stdexec)

add_library(FR::json-rpc ALIAS json-rpc)

add_subdirectory(test)
