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
#include <fr/autocereal/autocereal.h>
#include <fr/json-rpc/Error.h>
#include <meta>
#include <sstream>

namespace fr::json_rpc {

  struct Error;

  /**
   * unwrapRequestJson archives the individual elements of the structure
   * individually so we get a correct JSON string for this response.
   * autocereal likes to wrap objects encoded with my to_json function
   * in an outer "value0" shell. Gonna have to fix that one of these
   * days.
   *
   * Plus I need to change the error/result string based on the result.
   * And I don't want to risk bumping heads with autocereal by trying
   * to write a separate save function for these objects. It doesn't
   * like having two encoding messages.
   */
  template <typename ResponseType>
  std::string unwrapResponseJson(const ResponseType& response) {
    std::stringstream stream;
    {
      cereal::JSONOutputArchive ar(stream);
      ar(cereal::make_nvp("jsonrpc", response.jsonrpc));
      if constexpr (std::is_same_v<typename ResponseType::resultType, Error>) {
        ar(cereal::make_nvp("error", response.result));
      } else {
        ar(cereal::make_nvp("result", response.result));
      }
      if constexpr (std::meta::type_of(^^ResponseType::id) != ^^std::nullopt) {
        if constexpr (std::meta::type_of(^^ResponseType::id) != ^^std::nullptr_t) {
          ar(cereal::make_nvp("id", response.id));
        } else {
          ar(cereal::make_nvp("id", "null"));
        }
      }
    }
    return stream.str();
  }

  /**
   * Response type. This has all the things a JSON RPC Response needs!
   * Make ResultType an Error if it's... an error. Feels like that
   * should be obvious, but I thought I'd mention it.
   */
  template <typename ResultType, typename IdType>
  struct Response {
    using resultType = ResultType;
    std::string jsonrpc = "2.0";
    ResultType result;
    IdType id;
  };

}
