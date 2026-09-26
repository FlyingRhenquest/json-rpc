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
#include <fr/json-rpc/Server.h>
#include <fr/json-rpc/Client.h>

int add(int x, int y) {
  return x+y;
}

int subtract(int x, int y) {
  return x - y;
}

std::string sayHello(std::string name) {
  return std::string("Hello, ") + name;
}

int Steve(int x, int y);

TEST(ServerTest, Basic) {
  fr::json_rpc::Server<^^add, ^^subtract, ^^sayHello> services;
  fr::json_rpc::Client<^^add> addClient;
  fr::json_rpc::Client<^^subtract> subtractClient;
  fr::json_rpc::Client<^^Steve> steveClient;
  fr::json_rpc::Client<^^sayHello> helloClient;
  std::string helloRequest = helloClient.makeRequest(std::string("helloTest"), "Worlt");
  std::string addRequest = addClient.makeRequest(std::string("addId"), 2, 2);
  auto addResult = services.request<std::string>(addRequest);
  std::cout << addResult << std::endl;
  EXPECT_EQ(addResult, fr::json_rpc::unwrapResponseJson(fr::json_rpc::Response<int, std::string>{ . result = 4, .id = "addId"}));
  std::string subRequest = subtractClient.makeRequest(4,2,2);
  auto subResult = services.request<int>(subRequest);
  std::cout << subResult << std::endl;
  EXPECT_EQ(subResult, fr::json_rpc::unwrapResponseJson(fr::json_rpc::Response<int, int>{.result = 0, .id = 4}));
  std::string steveRequest = steveClient.makeRequest(std::string("Nope"), 4, 4);
  auto noSuchMethod = services.request<std::string>(steveRequest);
  EXPECT_EQ(noSuchMethod, fr::json_rpc::unwrapResponseJson(fr::json_rpc::Response<fr::json_rpc::Error, std::nullptr_t>{.result = { .code = -32601, .message = "Method \"Steve\" not found"}, .id = nullptr}));
  std::cout << noSuchMethod << std::endl;
  auto helloResponse = services.request<std::string>(helloRequest);
  std::cout << helloResponse << std::endl;
}
