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
#include <fr/json-rpc/Params.h>
#include <string>

std::string hello(std::string name) {
  return std::string("Hello, ") + name;
}

TEST(ParamsTest, Basic) {
  std::string steve("Steve");
  fr::json_rpc::Params<^^hello> params(steve);
  const std::string json = params.encode();
  fr::json_rpc::Params<^^hello> copy;
  copy.decode(json);
  EXPECT_EQ(params.impl.name, copy.impl.name);
}
