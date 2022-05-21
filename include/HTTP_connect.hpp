// Copyright 2022 Evgenzayts evgenzaytsev2002@yandex.ru

#ifndef INCLUDE_HTTP_CONNECT_HPP_
#define INCLUDE_HTTP_CONNECT_HPP_

#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "Suggestion.hpp"

#define TARGET "/v1/api/suggest"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class http_connection : public std::enable_shared_from_this<http_connection> {
 private:
  tcp::socket _socket;         // The socket for the currently connected client
  beast::flat_buffer _buffer{8192};    // The buffer for performing reads
  http::request<http::string_body> _request;    // The request message
  http::response<http::dynamic_body> _response; // The response message
  // The timer for putting a deadline on connection processing
  net::steady_timer _deadline{_socket.get_executor(),
                              std::chrono::seconds(60)};

 public:
  explicit http_connection(tcp::socket socket);

  void start(); // Initiate the asynchronous operations

 private:
  void request_read();    // Asynchronously receive a complete request message
  void request_working(); // Determine what needs to be done with the request
  void response_create(); // Construct a response message
  void response_send();   // Asynchronously transmit the response message
  void check_deadline();  // Check enough time on this connection
};

void http_server(tcp::acceptor& acceptor, tcp::socket& socket); // loop forever accepting new connections

#endif  // INCLUDE_HTTP_CONNECT_HPP_
