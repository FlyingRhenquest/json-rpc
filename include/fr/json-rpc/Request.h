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

#include <fr/json-rpc/Function.h>
#include <meta>
#include <optional>
#include <rfl.hpp>
#include <string>

namespace fr::json_rpc {

  /**
   * Request for a specific function.
   *
   * Note that the function does not actually need to be
   * defined anywhere in your executable, only declared.
   * So if you have a header for it, you can encode
   * a request for the function.
   *
   * This uses an optional reflect-cpp Generic object
   * for ID, since ID can be optional and can also
   * be null, int or a string. If you do not want
   * an ID in your request, used std::nullopt for the
   * ID. If you want it to be null, use std::monostate.
   */

  template <std::meta::info Freflect> struct Request {
    using ParamsType = Function<Freflect>::Params;
    using IdType = std::optional<rfl::Generic>;
    std::string jsonrpc = "2.0";
    std::string method = std::string{std::meta::identifier_of(Freflect)};
    std::optional<ParamsType> params = std::nullopt;
    IdType id;

    /**
     * Create a request for a function with the arguments supplied.
     * This object can be serialized immediately with rfl::json::write
     * after it's been created.
     *
     * If you want ID to not show up at all in the JSON, use std::nullopt.
     * If you want ID to be a null, use std::monostate.
     */
    template <typename... Args>
    constexpr Request(IdType id, Args&&... args) : id(id) {
      if constexpr (0 != Function<Freflect>::nparams) {
        ParamsType p;
        Function<Freflect>::initParams(p, args...);
        params = p;
      }
    }

    /**
     * Create an empty request (Params and ID must be initted before use)
     */
    Request() {}
  };

}
