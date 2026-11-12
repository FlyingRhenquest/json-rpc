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

#include <fr/json-rpc/Client.h>
#include <fr/json-rpc/Server.h>
#include <stdexec/execution.hpp>

namespace fr::json_rpc::pipelines {

  /**
   * A pipeline to run a client and server in the same process. This
   * just handles the creation of temporary strings that we'd
   * normally do when schlepping values back and forth between
   * the client and server as illustrated in the first server test.
   */

  template <typename Client, typename Server, typename... Args>
  auto InProcess(Client& client,
                 Server& server,
                 typename Client::IdType id,
                 Args&&... args) {
    return stdexec::just(client.encode(id, args...)) |
      stdexec::then([&server](std::string request) {
        return server.request(std::move(request));
      }) |
      stdexec::then([&client](std::string response) {
        return std::move(client.decode(std::move(response)));
      });
  }

}
