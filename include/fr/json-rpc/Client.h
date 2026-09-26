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
#include <fr/json-rpc/Request.h>
#include <meta>
#include <sstream>

namespace fr::json_rpc {

  /**
   * A function client. This client encodes a JSON string that can be pased to
   * Server::request to decode and run the request.
   *
   * You don't need an actual implementation of Funcion, just a declaration.
   * So if you have a header that declares the function, you can create a client
   * for it.
   */

  template <std::meta::info Function>
  struct Client {
    /**
     * Use std::nullopt for ID if you want none. Use std::nullptr_t for null.
     */
    template <typename IdType, typename... Args> std::string makeRequest(IdType id, Args&&... args) {
      return unwrapRequestJson(Request<Function, IdType>(id, args...));
    }
  };
  
}
