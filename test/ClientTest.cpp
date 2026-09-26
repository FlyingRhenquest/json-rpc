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

int add(int x, int y);
int subtract(int x, int y);

TEST(ClientTest, BasicTest) {
  fr::json_rpc::Client<^^add> addClient;
  std::cout << addClient.makeRequest(std::string("SomeId"), 4, 4) << std::endl;
}
