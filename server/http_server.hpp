#pragma once

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Util/ServerApplication.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::ServerSocket;
using Poco::Util::ServerApplication;

#include "http_request_factory.hpp"

class HTTPUserServer : public Poco::Util::ServerApplication {
 public:
  HTTPUserServer() = default;
  ~HTTPUserServer() override = default;

 protected:
  void initialize(Application& self) override {
    loadConfiguration();
    ServerApplication::initialize(self);
  }

  void uninitialize() override {
    ServerApplication::uninitialize();
  }

  int main(const std::vector<std::string>& args) {
    ServerSocket svs(Poco::Net::SocketAddress("127.0.0.1", 8080));
    HTTPServer srv(new HTTPRequestFactory(), svs, new HTTPServerParams());
    
    srv.start();
    waitForTerminationRequest();
    srv.stop();
    
    return Application::EXIT_OK;
  }
};
