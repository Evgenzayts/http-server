// Copyright 2022 Evgenzayts evgenzaytsev2002@yandex.ru

#include "HTTP_connect.hpp"

const std::shared_ptr<std::timed_mutex> mutex =
    std::make_shared<std::timed_mutex>();
std::shared_ptr<InfoJson> info_json = std::make_shared<InfoJson>();

void UpdateInfo() {
  while (true) {
    mutex->lock();
    info_json->LoadInfo();
    mutex->unlock();

    std::this_thread::sleep_for(std::chrono::minutes(15));
  }
}

http_connection::http_connection(tcp::socket socket)
    :  _socket(std::move(socket)) {}

void http_connection::start() {
  request_read();
  check_deadline();
}
void http_connection::request_read() {
  auto self = shared_from_this();

  http::async_read(_socket,
                   _buffer,
                   _request,
                   [self](beast::error_code error_code,
                          std::size_t bytes_transferred)
                   {
                     boost::ignore_unused(bytes_transferred);
                     if (!error_code) self->request_working();
                   });
}

void http_connection::request_working() {
  _response.version(_request.version());
  _response.keep_alive(false);

  switch (_request.method())
  {
    case http::verb::post:
      _response.result(http::status::ok);
      _response.set(http::field::server, "Beast");
      response_create();
      break;

    default:
      _response.result(http::status::bad_request);
      _response.set(http::field::content_type, "text/plain");
      beast::ostream(_response.body())
          << "Invalid request-method '"
          << std::string(_request.method_string())
          << "'";
      break;
  }

  response_send();
}

void http_connection::response_create() {
  if (_request.target() == TARGET) {
    _response.set(http::field::content_type, "application/json");

    mutex->lock();
    auto suggestion = new Suggestion(info_json->GetJson());
    mutex->unlock();

    json input_json = json::parse(_request.body());
    beast::ostream(_response.body()) <<
        suggestion->GetSuggest(input_json).dump(3);
  } else {
    _response.result(http::status::not_found);
    _response.set(http::field::content_type, "text/plain");
    beast::ostream(_response.body()) << "File or method not found\r\n";
  }
}

void http_connection::response_send() {
  auto self = shared_from_this();

  _response.content_length(_response.body().size());

  http::async_write(_socket,
                    _response,
                    [self](beast::error_code error_code, std::size_t)
                    {
                      self->_socket.shutdown(tcp::socket::shutdown_send,
                                             error_code);
                      self->_deadline.cancel();
                    });
}

void http_connection::check_deadline() {
  auto self = shared_from_this();

  _deadline.async_wait([self](beast::error_code error_code)
                       {
                         if (!error_code) {
                           // Close socket to cancel any outstanding operation
                           self->_socket.close(error_code);
                         }
                       });
}

void http_server(tcp::acceptor& acceptor, tcp::socket& socket) {
  info_json->LoadInfo();

  std::thread{UpdateInfo}.detach();

  acceptor.async_accept(socket,
                        [&](beast::error_code error_code)
                        {
                          if (!error_code)
                            std::make_shared<http_connection>(
                                std::move(socket) )->start();
                          http_server(acceptor, socket);
                        });
}
