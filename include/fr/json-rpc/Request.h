/*
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
#include <fr/json-rpc/Helpers.h>
#include <meta>
#include <sstream>
#include <stdexcept>

namespace fr::json_rpc {

  /**
   * unwrapResponseJson encodes the Response as a JSON string. We need
   * special handling for the various bits and don't want to write a
   * archiver for it as autocereal already has one and will get confused.
   */
  template <typename RequestType>
  std::string unwrapRequestJson(const RequestType& request) {
    static const std::string method(RequestType::method);
    std::stringstream stream;
    {
      cereal::JSONOutputArchive ar(stream);
      ar(cereal::make_nvp("jsonrpc", request.jsonrpc));
      ar(cereal::make_nvp("method", method));
      if constexpr (!RequestType::ParamsType::isEmpty) {
        ar(cereal::make_nvp("params", request.params.impl));
      }
      if constexpr (std::meta::type_of(^^RequestType::id) != ^^std::nullopt) {
        if constexpr (std::meta::type_of(^^RequestType::id) != ^^std::nullptr_t) {
          ar(cereal::make_nvp("id", request.id));
        } else {
          ar(cereal::make_nvp("id", "null"));
        }
      }
    }
    return stream.str();
  }

  // Unpack the method name from a request JSON
  inline const std::string requestRouting(const std::string &json) {
    std::stringstream stream(json);
    std::string method;
    {
      cereal::JSONInputArchive ar(stream);
      std::string rpcver;
      ar(rpcver);
      ar(method);
    }
    return method;
  }

  /**
   * Request type
   */
  template <std::meta::info Function, typename IdType>
  struct Request {
    using ParamsType = Params<Function>;
    std::string jsonrpc = "2.0";
    static constexpr auto method = to_fixed_string<std::meta::identifier_of(Function).size()>(std::meta::identifier_of(Function));
    ParamsType params;
    IdType id;

    template <typename... Args>
    constexpr Request(IdType id, Args&&... args) : params(args...), id(id) {}

    Request() {}

    void decode(const std::string& json) {
      std::stringstream stream(json);
      {
        cereal::JSONInputArchive ar(stream);
        std::string rpcver;
        ar(rpcver);
        if (rpcver != jsonrpc) {
          Error err(static_cast<int>(ERRORS::INVALID_REQUEST), "Unspported JSON-RPC Version");
          throw err;
        }
        std::string m;
        ar(m);
        if (method != std::string_view(m)) {
                    Error err(static_cast<int>(ERRORS::METHOD_NOT_FOUND), "Invalid method name");
          throw err;
        }
        if constexpr (!ParamsType::isEmpty) {
          ar(params.impl);
        }
        if constexpr (std::meta::type_of(^^id) != ^^std::nullopt) {
          if constexpr (std::meta::type_of(^^id) != ^^std::nullptr_t) {
            ar(id);
          } else {
            id = nullptr;
          }
        }
      }
    }
  };

}
