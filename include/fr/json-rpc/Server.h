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
#include <fr/json-rpc/Function.h>
#include <fr/json-rpc/Helpers.h>
#include <fr/json-rpc/Request.h>
#include <fr/json-rpc/Response.h>
#include <functional>
#include <meta>
#include <ranges>

namespace fr::json_rpc {

  /**
   * Server can handle calls for multiple functions. Current design matches on
   * function name, so overloading isn't allowed.
   */
  template <std::meta::info... ServiceObjects>
  class Server {
    // Struct for use by reflect-cpp to extract just the method name from the
    // request.
    using IdType = std::optional<rfl::Generic>;

    struct Method {
      std::string method;
    };

    // Struct used by reflect-cpp to extract just the ID from the request
    struct Id {
      IdType id;
    };

    const std::string getMethod(const std::string& json) {
      std::string method;
      auto m = rfl::json::read<Method>(json);
      if (m.has_value()) {
        method = m->method;
      };
      return method;
    }

    void errorFn() {};

  public:

    std::string request(std::string req) {
      std::string method = getMethod(req);
      std::string result = "";
      template for (constexpr size_t i : std::views::iota(0u, sizeof...(ServiceObjects))) {
        if (method == std::meta::identifier_of(ServiceObjects...[i])) {
          using fn = [: std::meta::type_of(ServiceObjects...[i]) :];
          auto request = rfl::json::read<Request<ServiceObjects...[i]>>(req);
          if (request.has_value()) {
            Response<ServiceObjects...[i]> response;
            response.id = request->id;
            Function<ServiceObjects...[i]> function;
            function.setParams(*request->params);
            if constexpr (std::is_void_v<typename Response<ServiceObjects...[i]>::ResultType>) {
              try {
                std::function_ref<typename Function<ServiceObjects...[i]>::Signature> ref = [: ServiceObjects...[i] :];
                std::apply(ref, function.tieParams());
              } catch (std::exception &e) {
                response.Error = Error{ERRORS::INTERNAL_ERROR, e.what()};
              }
            } else {
              try {
                std::function_ref<typename Function<ServiceObjects...[i]>::Signature> ref = [: ServiceObjects...[i] :];
                response.result = std::apply(ref, function.tieParams());
              } catch (std::exception &e) {
                response.error = Error{ERRORS::INTERNAL_ERROR, e.what()};
              }
            }
            result = rfl::json::write(response);
          }
        }
      }
      if (result.empty()) {
        auto id = rfl::json::read<Id>(req);
        if (id.has_value()) {
          Response<^^errorFn> response;
          response.id = id.value().id;
          response.error = Error{ERRORS::METHOD_NOT_FOUND, method};
          result = rfl::json::write(response);
        }
      }
      return result;
    }
  };

}
