#include "HTTP_connect.hpp"

int main(int argc, char* argv[]) {
  try {
    if(argc != 3) // Check command line arguments
    {
      std::cerr << "Usage: " << argv[0] << " <address> <port>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80\n";
      return EXIT_FAILURE;
    }

    auto const address = net::ip::make_address(argv[1]);
    auto port = static_cast<unsigned short>(std::atoi(argv[2]));

    net::io_context ioc{1};
    tcp::acceptor acceptor{ioc, {address, port}};
    tcp::socket socket{ioc};
    http_server(acceptor, socket);

    ioc.run();
  } catch(std::exception const& exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}
