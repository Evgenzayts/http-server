//
// Created by ubuntu on 5/16/22.
//

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
#include "Suggestion.hpp"

#define TARGET "/v1/api/suggest"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class http_connection : public std::enable_shared_from_this<http_connection> {
 private:
  tcp::socket _socket;                          // The socket for the currently connected client
  beast::flat_buffer _buffer{8192};        // The buffer for performing reads
  //http::request<http::dynamic_body> _request;
  http::request<http::string_body> _request;    // The request message
  http::response<http::dynamic_body> _response; // The response message
  // The timer for putting a deadline on connection processing
  net::steady_timer _deadline{_socket.get_executor(),
                              std::chrono::seconds(60)};

 public:
  explicit http_connection(tcp::socket socket);

  void start(std::shared_ptr<std::timed_mutex>& mutex,
             std::shared_ptr<InfoJson>& info_json); // Initiate the asynchronous operations associated with the connection

 private:
  void request_read(std::shared_ptr<std::timed_mutex>& mutex,
                    std::shared_ptr<InfoJson>& info_json);    // Asynchronously receive a complete request message
  void request_working(std::shared_ptr<std::timed_mutex>& mutex,
                       std::shared_ptr<InfoJson>& info_json); // Determine what needs to be done with the request message
  void response_create(std::shared_ptr<std::timed_mutex>& mutex,
                       std::shared_ptr<InfoJson>& info_json); // Construct a response message based on the program state
  void response_send();   // Asynchronously transmit the response message
  void check_deadline();  // Check whether we have spent enough time on this connection
};

void http_server(tcp::acceptor& acceptor, tcp::socket& socket); // loop forever accepting new connections

#endif  // INCLUDE_HTTP_CONNECT_HPP_
