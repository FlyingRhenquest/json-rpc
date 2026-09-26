find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()

set(TEST_SRC
  "${CMAKE_CURRENT_SOURCE_DIR}/ClientTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ServerTest.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ParamsTest.cpp"
)

add_executable(test
  ${TEST_SRC}
)

target_link_libraries(test PUBLIC
  GTest::GTest
  GTest::Main
  FR::json-rpc
)
