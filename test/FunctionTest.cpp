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
#include <fr/json-rpc/Function.h>
#include <string>

// Declare function only (Don't need to implement)
std::string FunctionTestFunction();
std::string FunctionTestHello(std::string name);

TEST(FunctionTest, Def) {
  using f = fr::json_rpc::Function<^^FunctionTestFunction>;
  ASSERT_EQ(f::name, "FunctionTestFunction");
  ASSERT_EQ(f::nparams, 0l);
  using f2 = fr::json_rpc::Function<^^FunctionTestHello>;
  ASSERT_EQ(f2::name, "FunctionTestHello");
  ASSERT_EQ(f2::nparams, 1l);
}

TEST(FunctionTest, InitParams) {
  using f = fr::json_rpc::Function<^^FunctionTestHello>;
  f fs;
  f::initParams(fs.params, std::string("Steve"));
  ASSERT_EQ(fs.params.name, "Steve");
}

TEST(FunctionTest, TieParams) {
  using f = fr::json_rpc::Function<^^FunctionTestHello>;
  f fs;
  f::initParams(fs.params, std::string("Steve"));
  ASSERT_EQ(std::get<0>(fs.tieParams()), "Steve");
}
