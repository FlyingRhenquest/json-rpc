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
#include <fixed_string.hpp>
#include <fr/json-rpc/Helpers.h>
#include <functional>
#include <meta>
#include <ranges>
#include <vector>

namespace fr::json_rpc {

  /**
   * A Function sets up parameters for a function. It reflects the
   * function at compile time and its Params is defined with
   * define_aggregate to a struct that holds the parameters for the
   * Function.
   */

  template <std::meta::info Freflect>
  struct Function {
    /**
     * Function name
     */
    static constexpr auto name = std::meta::identifier_of(Freflect);
    static constexpr size_t nparams = std::meta::parameters_of(Freflect).size();

    /**
     * Params will actually be the part of this structure
     * that is used elsewhere. Declare with `Function<function>::Params params;`
     * for the serialziation to work correctly.
     */
    struct Params;
    using ReturnType = [: std::meta::return_type_of(Freflect) :];
    using Signature = [: std::meta::type_of(Freflect) :];

    consteval {
      std::vector<std::meta::info> handlerSpecs;
      template for (constexpr auto arg : std::define_static_array(std::meta::parameters_of(Freflect))) {
        constexpr auto argType = std::meta::type_of(arg);
        constexpr auto argName = std::meta::identifier_of(arg);
        handlerSpecs.push_back(std::meta::data_member_spec(argType, { .name = argName }));
      }
      std::meta::define_aggregate(^^Params, handlerSpecs);
    }

    Params params;

    /**
     * initParams initializes a params structure for this function.
     * We'll do a refrerence version that degradeds to this pointer
     * version. If we ever use it with a unique or shared pointer
     * we could add an overload for that too.
     */
    template <typename... Args>
    constexpr static void initParams(Params* p, Args&&... args) {
      constexpr static auto ctx = std::meta::access_context::unchecked();
      constexpr auto argRef = std::define_static_array(std::meta::members_of(^^Params, ctx));
      /*
       * We iterate over all the args and our argRef array, which should be the
       * same size, and set each member of the Params object to its associated
       * argument value using splices indexed on an iota (This is one way to
       * do indexed iteration at compile time.)
       */
      template for (constexpr std::size_t i : std::views::iota(0u, sizeof...(Args))) {
        p->[: argRef[i] :] = std::forward<Args...[i]>(args...[i]);
      }
    }

    /**
     * initParams Reference version (This degrades to the pointer version)
     */
    template <typename... Args>
    constexpr static void initParams(Params& p, Args&&... args) {
      initParams(&p, args...);
    }

    struct MemberPack {
      static constexpr auto members = std::define_static_array(std::meta::nonstatic_data_members_of(^^Params, std::meta::access_context::unchecked()));
    };

    /**
     * Set parameters from another parameters
     */
    void setParams(Params& p) {
      template for (constexpr auto i : std::views::iota(0u, MemberPack::members.size())) {
        params.[: MemberPack::members[i] :] = p.[: MemberPack::members[i] :];
      }
    }

    /**
     * tieParams returns a tuple of parameters from params so that we can
     * call a function with the return value, using std::apply.
     */

    auto tieParams() {
      return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return std::tie(params.[: MemberPack::members[I] :]...);
      }(std::make_index_sequence<MemberPack::members.size()>{});
    }

  };

}
