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
#include <meta>
#include <optional>
#include <rfl.hpp>
#include <string>
#include <type_traits>
#include <vector>

namespace fr::json_rpc {

  /**
   * You can pass this a function reflection or a response type reflection.
   * The function refleciton will figure out the return type of the function.
   * Generally you would use the an Error reflection if an error occurs.
   */
  template <std::meta::info Freflect>
  struct Response {

  private:
    struct Info;

    consteval {
      std::vector<std::meta::info> spec;
      if constexpr (std::meta::is_function(Freflect)) {
        constexpr auto responseType = std::meta::return_type_of(Freflect);
        constexpr auto identity = std::meta::substitute(^^std::type_identity, { responseType });
        spec.push_back(std::meta::data_member_spec(identity, { .name = "ret" }));
        if constexpr (is_void_type(responseType)) {
          constexpr auto sanitizedResponseType = ^^bool;
          constexpr auto sidentity = std::meta::substitute(^^std::type_identity, { sanitizedResponseType});
          spec.push_back(std::meta::data_member_spec(sidentity, { .name = "s_ret"}));
        } else {
          spec.push_back(std::meta::data_member_spec(identity, { .name = "s_ret"}));
        }
      } else if constexpr (std::meta::is_type(Freflect)) {
        constexpr auto identity = std::meta::substitute(^^std::type_identity, { Freflect });
        spec.push_back(std::meta::data_member_spec(identity, { .name = "ret" }));
        spec.push_back(std::meta::data_member_spec(identity, { .name = "s_ret" }));
      } else {
        static_assert(false, "Response type is not a function or a type");
      }
      std::meta::define_aggregate(^^Info, spec);
    }

    using IdentityType = typename [: std::meta::type_of(std::meta::nonstatic_data_members_of(^^Info, std::meta::access_context::unchecked())[0]) :];
    // Cant use IdentityType in optional if return type is void
    using SanitizedIdentityType = typename [: std::meta::type_of(std::meta::nonstatic_data_members_of(^^Info, std::meta::access_context::unchecked())[1]) :];
    using SanitizedResultType = SanitizedIdentityType::type;

  public:
    using ResultType = IdentityType::type;
    using IdType = std::optional<rfl::Generic>;
    std::string jsonrpc = "2.0";
    std::optional<SanitizedResultType> result = std::nullopt;
    std::optional<Error> error = std::nullopt;
    IdType id = std::nullopt;

    constexpr Response(IdType id) : id(id) {
    }

    constexpr Response(IdType id, SanitizedResultType res) : id(id) {
      if constexpr (std::is_same_v<Error, SanitizedResultType>) {
        error = res;
      } else {
        result = res;
      }
    }

    constexpr Response() {}

  };

}
