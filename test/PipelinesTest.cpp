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
#include <fr/json-rpc/Pipelines.h>

std::string PipelinesTestHello(std::string name) {
  return std::format("Hello, {}", name);
}

TEST(PipelinesTest, Basic) {

  fr::json_rpc::Client<^^PipelinesTestHello> client;
  fr::json_rpc::Server<^^PipelinesTestHello> server;

  auto pipeline = fr::json_rpc::pipelines::InProcess(client, server, "BasicTest", "Worlt!");
  auto [response] = stdexec::sync_wait(std::move(pipeline)).value();
  auto id = client.lastId();

  ASSERT_EQ(response, "Hello, Worlt!");
  ASSERT_EQ(id, "BasicTest");
}
