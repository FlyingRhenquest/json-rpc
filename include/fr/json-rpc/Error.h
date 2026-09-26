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

#include <string>

namespace fr::json_rpc {

  /**
   * Error types, if you want to use them. Autocereal also doesn't
   * know how to deal with these yet, so I'll make the code in Error a
   * plain ol' int. It's kind of a pain in the ass to have to
   * static_cast it, but I'd be inclined to make autocereal use the
   * identifier for enums anyway, and JSON RPC wants an int in the
   * error code.
   *
   */
  enum class ERRORS : int {
    PARSE_ERROR = -32700,
    INVALID_REQUEST = -32600,
    METHOD_NOT_FOUND = -32601,
    INVALID_PARAMS =  -32602,
    INTERNAL_ERROR = - 32603
  };

  /**
   * I am error
   */
  struct Error {
    int code;
    std::string message;
  };


}
