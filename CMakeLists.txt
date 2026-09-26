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

FetchContent_Declare(
  autocereal
  GIT_REPOSITORY https://github.com/FlyingRhenquest/autocereal.git
  GIT_TAG origin/main
  FIND_PACKAGE_ARGS CONFIG
)
set(AUTOCEREAL_BUILD_TESTS OFF)
FetchContent_MakeAvailable(autocereal)

set(INTERFACE_HEADERS
  "include/fr/json-rpc/Server.h"
)

add_library(json-rpc INTERFACE)

target_include_directories(json-rpc INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(json-rpc INTERFACE fixed_string::fixed_string FR::autocereal)

add_library(FR::json-rpc ALIAS json-rpc)

add_subdirectory(test)

#add_subdirectory(examples)
