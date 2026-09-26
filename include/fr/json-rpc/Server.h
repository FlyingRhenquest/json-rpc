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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#pragma once
#include <array>
#include <fixed_string.hpp>
#include <functional>
#include <format>
#include <meta>
#include <fr/autocereal/autocereal.h>
#include <fr/json-rpc/Helpers.h>
#include <fr/json-rpc/Request.h>
#include <fr/json-rpc/Response.h>
#include <string>

namespace fr::json_rpc {

  /**
   * ServiceHandler; created with the name of the method and a function
   * ref to the function to be called.
   */
  template <fixstr::fixed_string Method, std::meta::info Function>
  struct ServiceHandler {
    struct handler;
    static constexpr auto name = Method;
    using FunctionSig = [: std::meta::type_of(Function) :];
    using ReturnType = typename[: std::meta::return_type_of(Function) :];

    std::function_ref<FunctionSig> function  = [: Function :];

    /**
     * Call the function with some arguments. We encode to json here
     * so the return type can be the same in all cases. If you want to
     * customize your error response, you can throw a json_rpc::Error
     * and I'll wrap it in a Response for you.
     */
    template <typename IdType>
    std::string call(const std::string& req) {
      Request<Function, IdType> request;
      try {
        request.decode(req);
        Response<ReturnType, IdType> response{
          .result = std::apply(function, request.params.tieParams()),
          .id = request.id
        };
        return unwrapResponseJson(response);
      } catch (std::exception &e) {
        Error err{
          .code = static_cast<int>(ERRORS::INTERNAL_ERROR),
          .message = e.what()
        };
        Response<Error, IdType> whoopsie {
          .result = err,
          .id = request.id
        };
        return unwrapResponseJson(whoopsie);
      } catch (Error &e) {
        Response<Error, IdType> whoopsie {
          .result = e,
          .id = request.id
        };
        return unwrapResponseJson(whoopsie);
      }
      return std::string("");
    }

  };

  /**
   * A JSON RPC Server. Um... backend kinda thing. It doesn't
   * actually have a protocol. But you can stick functions
   * in it and then call its "request" method and get
   * a JSON-RPC response back, so that's *most* of the battle,
   * anyway. I'll probably handle the protocol with proxy objects
   * or something that take terminal input or pistache routes
   * and converts them into request calls to this Server
   * object.
   *
   * How do you... oh yeah. Write a function,
   * create a server with a reflection to your function
   * and then call server.request(id, function name, function parameters...)
   *
   * The JSON RPC guys thought they'd be cute and let ID be multiple
   * possible types, so ID can be multiple possible types. Cereal doesn't
   * like char arrays, so if you want your IDs to be strings, make sure
   * C++ knows they're std::strings.
   */
  template <auto... ServiceObjects>
  class Server {
  protected:
    struct Handlers;

    /**
     * Unwrap all my service object function reflections and rebundle them
     * as service handlers with std::function_refs pointing to the functions.
     * Stuff them all in a Handlers structure, with the service handler
     * name for that specific function being the name of the function.
     * So if you create a Server<^^add>, handlers will have one "add" member,
     * which is a ServiceHandler and which holds a std::function_ref that
     * points at your function.
     */
    consteval {
      std::vector<std::meta::info> handlerSpecs;
      template for (constexpr auto ServiceObject : ServicePack<ServiceObjects...>::objects) {
        if constexpr (std::meta::is_function(ServiceObject)) {
          constexpr auto name = std::meta::identifier_of(ServiceObject);
          constexpr auto methodName = to_fixed_string<name.size()>(name);
          constexpr auto functionValue =
            std::meta::reflect_constant(&[:ServiceObject:]);
          constexpr auto HandlerTypeInfo = std::meta::substitute(^^ServiceHandler, {
              std::meta::reflect_constant(methodName),
              ^^ServiceObject
            });
          handlerSpecs.push_back(std::meta::data_member_spec(HandlerTypeInfo, {.name = methodName}));
        } else {
          static_assert(false, "Object types other than functions are not supported yet.");
        }
      }
      std::meta::define_aggregate(^^Handlers, handlerSpecs);
    }

    /**
     * Actually create a handlers member for all the handlers we
     * just defiend in that consteval loop up there.
     */
    Handlers handlers;
  public:
    /**
     * Make a request to a function that this server handles. Returns a response from a function
     * or an error response.
     */
    template <typename IdType>
    constexpr std::string request(const std::string& req) {
      constexpr static auto ctx = std::meta::access_context::unchecked();
      constexpr static auto reflections = std::define_static_array(std::meta::nonstatic_data_members_of(^^Handlers, ctx));
      const std::string routing = requestRouting(req);
      std::string_view routingView{routing};
      // Iterate through the handlers we created and see if we have a match for the
      // requested name. If so, we can just substitute the handler to call
      // the function it's pointing at, which is nice and convenient
      template for (constexpr auto handler : reflections) {
        if (routingView == std::meta::identifier_of(handler)) {
          return handlers.[: handler :].template call<IdType>(req);
        }
      }
      Error err{
        .code = static_cast<int>(ERRORS::METHOD_NOT_FOUND),
        .message = std::format("Method \"{}\" not found", routing)
      };
      Response<Error, std::nullptr_t> response {
        .result = err,
        .id = nullptr
      };
      return unwrapResponseJson(response);
    }
  };

}
