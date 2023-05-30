#pragma once

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "http_request_factory.hpp"
//#include "../database/user.h"

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
    Report::crate_report_table();

    ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8080));
    HTTPServer srv(new HTTPRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
    
    srv.start();
    waitForTerminationRequest();
    srv.stop();
    
    return Application::EXIT_OK;
  }
};
