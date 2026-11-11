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

#pragma once
#include <fixed_string.hpp>
#include <meta>
#include <string_view>

namespace fr::json_rpc {

  /**
   * Convert a std::string_view to a fixstr::fixed_string.
   * This lets me store function names from meta identifier_ofs
   * for my routing
   */
  template <std::size_t N>
  consteval auto to_fixed_string(std::string_view sv) {
    char chars[N + 1]{};

    for (std::size_t i = 0; i < N; ++i)
      chars[i] = sv[i];

    return fixstr::fixed_string(chars);
  }

}
