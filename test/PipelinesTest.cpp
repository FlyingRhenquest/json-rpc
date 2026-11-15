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

#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <exec/static_thread_pool.hpp>
#include <exec/asio/use_sender.hpp>
#include <format>
#include <fr/json-rpc/Pipelines.h>
#include <iostream>
#include <stdexec/execution.hpp>
#include <thread>
#include <unistd.h>

std::string PipelinesTestHello(std::string name) {
  return std::format("Hello, {}", name);
}

/**
 * Basic stdexec pipeline test -- The InProcess pipeline just calls encode
 * with a client, passes the string from that to encode from a server and
 * then calls decode on the client using the server response.
 */

TEST(PipelinesTest, Basic) {

  fr::json_rpc::Client<^^PipelinesTestHello> client;
  fr::json_rpc::Server<^^PipelinesTestHello> server;

  auto pipeline = fr::json_rpc::pipelines::InProcess(client, server, "BasicTest", "Worlt!");
  auto [response] = stdexec::sync_wait(std::move(pipeline)).value();
  auto id = client.lastId();

  ASSERT_EQ(response, "Hello, Worlt!");
  ASSERT_EQ(id, "BasicTest");
}

/**
 * Set up a standalone asio server that listens for and responds
 * to requests on a pipe. Write a client request over the pipe,
 * decode the result and make sure we got the expected result.
 */

TEST(PipelinesTest, ReadWriteAsio) {
  using tx_pipe = exec::asio::use_sender_t::as_default_on_t<boost::asio::writable_pipe>;
  using rx_pipe = exec::asio::use_sender_t::as_default_on_t<boost::asio::readable_pipe>;

  boost::asio::io_context io;
  // We want our IO context to run until we're done. A work_guard will keep
  // it from exiting until we reset the guard.
  auto guard = boost::asio::make_work_guard(io);
  std::jthread ioThread([&] { io.run(); });

  rx_pipe server_rx(io);
  tx_pipe server_tx(io);
  rx_pipe client_rx(io);
  tx_pipe client_tx(io);

  boost::asio::connect_pipe(server_rx, client_tx);
  boost::asio::connect_pipe(client_rx, server_tx);

  fr::json_rpc::Client<^^PipelinesTestHello> client;
  fr::json_rpc::Server<^^PipelinesTestHello> server;
  // Buffer strings for async reads
  std::string serverReadBuffer;
  std::string clientReadBuffer;

  // Set up pipelines
  //
  // Use stdexec::then when returning values and stdexec::let when returning senders
  // (boost::asio operations are also senders)

  auto serverPipeline =
    stdexec::just() |
    stdexec::then([&](){
      std::cout << "Server start, reading bytes..." << std::endl;
    }) | stdexec::let_value([&]() {
      return boost::asio::async_read_until(server_rx, boost::asio::dynamic_buffer(serverReadBuffer), '\n');
      }) |
    stdexec::then([&](std::size_t nbytesRead) {
      // Exclude '\n'
      std::string request = serverReadBuffer.substr(0, nbytesRead - 1);
      std::cout << "Server read request: " << request << std::endl;
      // Remove this request from the buffer
      serverReadBuffer.erase(0, nbytesRead);
      return server.request(std::move(request));
    }) | stdexec::let_value([&](std::string response) {
      response.append("\n");
      std::cout << "Server sending response: " << response << std::endl;
      // Keep response alive until the write completes
      return stdexec::just(std::move(response)) |
        stdexec::let_value([&](std::string& response) {
          // Send request back to client
          return boost::asio::async_write(server_tx, boost::asio::buffer(response));
        }) | stdexec::then([](std::size_t /* nbytesWritten */) {}); // Sets response type to void
    }) | stdexec::upon_error([](auto err) {
      std::cerr << "Server pipe error" << std::endl;
    });

  auto clientPipeline = stdexec::let_value([&](std::string& request) {
    request.append("\n");
    return boost::asio::async_write(client_tx, boost::asio::buffer(request));
  }) | stdexec::let_value([&](std::size_t nbytesWritten) {
    return boost::asio::async_read_until(client_rx, boost::asio::dynamic_buffer(clientReadBuffer), '\n');
  }) | stdexec::then([&](std::size_t nbytesRead) {
    std::string response = clientReadBuffer.substr(0, nbytesRead - 1);
    clientReadBuffer.erase(0, nbytesRead);
    return response;
  }) | stdexec::then([&](std::string response) {
    return client.decode(std::move(response));
  }) | stdexec::upon_error([](auto err) {
    std::cerr << "Client pipe error" << std::endl;
    return std::string{};
  });

  // Set up scheduler
  exec::static_thread_pool pool{4};
  auto scheduler = pool.get_scheduler();

  // Set up a request pipeline that runs the server for the one request,
  // makes the request and returns the result;
  auto makeRequest =
    stdexec::when_all
    (stdexec::starts_on(scheduler, stdexec::just(client.encode("requestId", "Steve")) |
                        clientPipeline),
     stdexec::starts_on(scheduler, serverPipeline)
    );

  auto [response] = stdexec::sync_wait(std::move(makeRequest)).value();

  // Shut down IO thread
  guard.reset();

  ASSERT_EQ(response, "Hello, Steve");

}
