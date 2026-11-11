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
#include <fr/json-rpc/Client.h>
#include <iostream>

// Declare some functions for the client
// These do not actually need to be implemented.

std::string ClientTestHello();

TEST(ClientTest, RoundTrip) {
  fr::json_rpc::Client<^^ClientTestHello> client;
  // Set request ID to "helloTest"
  std::string request = client.encode("helloTest");
  ASSERT_EQ(request, "{\"jsonrpc\":\"2.0\",\"method\":\"ClientTestHello\",\"id\":\"helloTest\"}");
  // Just set up a response to decode right now
  fr::json_rpc::Response<^^ClientTestHello> response{"helloTest", "Hello Worlt!"};
  std::string r = rfl::json::write(response);
  ASSERT_EQ(client.decode(r), "Hello Worlt!");
  ASSERT_EQ(client.lastId(), "helloTest");
}

TEST(ClientTest, Error) {
  fr::json_rpc::Client<^^ClientTestHello> client;
  fr::json_rpc::Response<^^ClientTestHello> response{"errorTest"};
  response.error = fr::json_rpc::Error{-1, "An error occurred"};
  std::string r = rfl::json::write(response);
  EXPECT_THROW(client.decode(r), fr::json_rpc::Error);
  ASSERT_EQ(client.lastId(), "errorTest");
}
