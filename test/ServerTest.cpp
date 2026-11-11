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
#include <format>
#include <fr/json-rpc/Client.h>
#include <fr/json-rpc/Server.h>

// We're actually implementing some functions this time

std::string ServerTestHello(std::string name) {
  return std::format("Hello, {}!", name);
}

int ServerTestPlusTwo(int i) {
  return i + 2;
}

TEST(ServerTest, RoundTrip) {
  fr::json_rpc::Server<^^ServerTestHello, ^^ServerTestPlusTwo> server;
  fr::json_rpc::Client<^^ServerTestHello> helloClient;
  std::string helloRequest = helloClient.encode(1, "Worlt");
  std::string helloResponse = server.request(helloRequest);
  std::string helloResult = helloClient.decode(helloResponse);
  ASSERT_EQ(helloResult, "Hello, Worlt!");
  ASSERT_EQ(helloClient.lastId(), 1);

  fr::json_rpc::Client<^^ServerTestPlusTwo> plusClient;
  std::string plusRequest = plusClient.encode(2, 42);
  std::string plusResponse = server.request(plusRequest);
  int plusResult = plusClient.decode(plusResponse);
  ASSERT_EQ(plusResult, 44);
  ASSERT_EQ(plusClient.lastId(), 2);
}
