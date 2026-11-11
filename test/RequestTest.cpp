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
#include <iostream>
#include <fr/json-rpc/Request.h>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <string>

// Only need to declare functions; they don't need to be defined
// unless we want to call them

void RequestTestFunction();
std::string RequestTestHello(std::string name);

TEST(RequestTest, NoParamsNoId) {
  fr::json_rpc::Request<^^RequestTestFunction> r;
  ASSERT_FALSE(r.params.has_value());
  ASSERT_FALSE(r.id.has_value());
  ASSERT_EQ(r.method, "RequestTestFunction");
  std::string json = rfl::json::write(r);
  ASSERT_EQ(json, "{\"jsonrpc\":\"2.0\",\"method\":\"RequestTestFunction\"}");
}

TEST(RequestType, NoParamsId) {
  fr::json_rpc::Request<^^RequestTestFunction> r{4};
  ASSERT_TRUE(r.id.has_value());
  ASSERT_EQ(r.id.value(), 4);
  std::string json = rfl::json::write(r);
  ASSERT_EQ(json, "{\"jsonrpc\":\"2.0\",\"method\":\"RequestTestFunction\",\"id\":4}");
  fr::json_rpc::Request<^^RequestTestFunction> rs{"test_function"};
  std::string jsons = rfl::json::write(rs);
  ASSERT_EQ(jsons, "{\"jsonrpc\":\"2.0\",\"method\":\"RequestTestFunction\",\"id\":\"test_function\"}");
}

TEST(RequestType, ParamsTest) {
  fr::json_rpc::Request<^^RequestTestHello> r{1, "Steve"};
  ASSERT_TRUE(r.params.has_value());
  ASSERT_EQ(r.params.value().name, "Steve");
  std::string json = rfl::json::write(r);
  ASSERT_EQ(json, "{\"jsonrpc\":\"2.0\",\"method\":\"RequestTestHello\",\"params\":{\"name\":\"Steve\"},\"id\":1}");
}
