#pragma once

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include <iostream>


using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPRequestHandler;

#include "../util/strings.hpp"
#include "handlers/report_handler.hpp"

class HTTPRequestFactory : public HTTPRequestHandlerFactory {
 public:
  HTTPRequestFactory() = default;

  HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) {
    std::cout << "request: [" << request.getURI() << "]" << std::endl;

    return new ReportHandler();
  }
};