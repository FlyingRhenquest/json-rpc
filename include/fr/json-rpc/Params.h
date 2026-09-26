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

#include <cctype>
#include <fr/autocereal/autocereal.h>
#include <fr/json-rpc/Helpers.h>
#include <iostream>
#include <ranges>
#include <meta>
#include <sstream>

namespace fr::json_rpc {

  template <std::meta::info Function>
  struct Parameters;

  /**
   * A JSON-RPC Parameters type for a function.
   */

  template <std::meta::info Function>
  class Params {
  public:
    struct Impl;
    /**
     * Unwrap args into impl.
     */
    consteval {
      std::vector<std::meta::info> handlerSpecs;
      template for (constexpr auto arg : Parameters<Function>::params) {
        constexpr auto argType = std::meta::type_of(arg);
        constexpr auto argName = std::meta::identifier_of(arg);
        handlerSpecs.push_back(std::meta::data_member_spec(argType, { .name = argName}));
      }
      std::meta::define_aggregate(^^Impl, handlerSpecs);
    }

    Impl impl;
    static constexpr bool isEmpty = std::meta::members_of(^^Impl, std::meta::access_context::unchecked()).size() == 0;

    template <typename... Args>
    constexpr Params(Args&&... args) {
      // Initialize all the things
      constexpr static auto ctx = std::meta::access_context::unchecked();
      constexpr auto argRef = std::define_static_array(std::meta::members_of(^^Impl, ctx));
      template for (constexpr std::size_t i : std::views::iota(0u, sizeof...(Args))) {
        impl.[: argRef[i] :] = std::forward<Args...[i]>(args...[i]);
      }
    }

    Params() {};

    /**
     * Encode this params to json. Request will probably just serialize impl,
     * though, rather than actually using this function. This is more handy
     * for testing.
     */

    std::string encode() {
      return fr::autocereal::to_json(cereal::make_nvp("params", impl));
    }

    /**
     * Populates impl with JSON data in an istream
     */
    void decode(std::istream& jsonStream) {
      fr::autocereal::from_json(impl, jsonStream);
    }

    /**
     * Do string too
     */
    void decode(const std::string& json) {
      std::stringstream str(json);
      decode(str);
    }

    // Is it my imagination or has g++ gotten worse about this?
    // I feel like you used to be able to get away with putting a
    // std::define_static_array in a constexpr function. Though
    // the function I'm using this in isn't constexpr...
    template <std::meta::info Impl>
    struct MemberPack {
      static constexpr auto members = std::define_static_array(std::meta::nonstatic_data_members_of(Impl, std::meta::access_context::unchecked()));
    };

    // Make a tuple of the parameters so I can invoke it with a
    // service object.
    auto tieParams() const {
      using members = MemberPack<^^Impl>;
      return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return std::tie(impl.[: members::members[I] :]...);
      }(std::make_index_sequence<members::members.size()>{});
    }
  };

}
