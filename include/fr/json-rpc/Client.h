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
#include <fr/json-rpc/Error.h>
#include <fr/json-rpc/Function.h>
#include <fr/json-rpc/Request.h>
#include <fr/json-rpc/Response.h>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <stdexcept>

namespace fr::json_rpc {

  /**
   * Our client. This takes a reflection of one function and encodes
   * a request to that function. It will also decode the response from
   * the server, if it receives one.
   *
   * @tparam Freflect - Function refleciton -- a std::meta::info for a function
   */

  template <std::meta::info Freflect>
  struct Client {
    using IdType = std::optional<rfl::Generic>;
    using ReturnType = [: std::meta::return_type_of(Freflect) :];

  private:
    IdType _lastId;

  public:

    /**
     * Return the last decoded ID
     */
    IdType lastId() { return _lastId; };
    /**
     * Encode a request
     *
     * @param id - A string or int. Use std::nullopt if you don't want one. Use
     *             std::monostate if you want it to encode as "null".
     * @param args... - Arguments for the function you want to call
     */
    template <typename... Args>
    std::string encode(IdType id, Args&&... args) {
      std::string ret;
      Request<Freflect> request(id, args...);
      ret = rfl::json::write(request);
      return ret;
    }

    /**
     * Decode a response. This will throw an Error if the response
     * contains an Error. You can query the ID of this call with
     * lastId(), until the next decode call.
     *
     */
    Response<Freflect>::ResultType decode(std::string response) {
      auto r = rfl::json::read<Response<Freflect>>(response);
      if (!r) {
        throw std::runtime_error("No response received");
      }
      _lastId = r->id;
      if (r->error.has_value()) {
        throw r->error.value();
      }
      if constexpr (std::is_void_v<typename Response<Freflect>::ResultType>) {
        return;
      }
      if (r->result.has_value()) {
        return r->result.value();
      }
      throw std::runtime_error("No expected return value");
    }
  };
}
