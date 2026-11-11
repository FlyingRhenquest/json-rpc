/**
 * Copyright 2026 Bruce Ide
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <fr/json-rpc/Error.h>
#include <fr/json-rpc/Response.h>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <type_traits>

// Declare test functions (They don't need to be defined)
void ResponseTestFunction();
std::string ResponseTestHello();

TEST(ResponseTest, VoidFn) {
  using ResponseType = fr::json_rpc::Response<^^ResponseTestFunction>;
  ASSERT_TRUE(std::is_void_v<ResponseType::ResultType>);
  ResponseType r;
  ASSERT_FALSE(r.result.has_value());
  ASSERT_FALSE(r.error.has_value());
  ASSERT_FALSE(r.id.has_value());
  std::string json = rfl::json::write(r);
  ASSERT_EQ(json, "{\"jsonrpc\":\"2.0\"}");
}

TEST(ResponseTest, VoidFnWithId) {
  using ResponseType = fr::json_rpc::Response<^^ResponseTestFunction>;
  ResponseType r{4};
  ASSERT_TRUE(r.id.has_value());
  ASSERT_EQ(r.id.value(), 4);
  std::string json = rfl::json::write(r);
  ASSERT_EQ(json, "{\"jsonrpc\":\"2.0\",\"id\":4}");
}

TEST(ResponseTest, WithReturn) {
  using ResponseType = fr::json_rpc::Response<^^ResponseTestHello>;
  ResponseType r{"test", "Hello Worlt!"};
  ASSERT_TRUE(r.id.has_value());
  ASSERT_TRUE(r.result.has_value());
  std::string json = rfl::json::write(r);
  ASSERT_EQ(json, "{\"jsonrpc\":\"2.0\",\"result\":\"Hello Worlt!\",\"id\":\"test\"}");
}

TEST(ResponseTest, ErrorType) {
  using ResponseType = fr::json_rpc::Response<^^fr::json_rpc::Error>;
  fr::json_rpc::Error e{fr::json_rpc::ERRORS::PARSE_ERROR, "Parse Error"};
  ResponseType r{1, e};
  ASSERT_FALSE(r.result.has_value());
  ASSERT_TRUE(r.error.has_value());
  std::string json = rfl::json::write(r);
  ASSERT_EQ(json, "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse Error\"},\"id\":1}");
}
