#pragma once

#include <iostream>
#include <iostream>
#include <fstream>

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTMLForm.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTMLForm;

#include "../../database/report.hpp"
#include "../../util/strings.hpp"
#include "../../service/report_service.hpp"

class ReportHandler : public HTTPRequestHandler {
 public:
  ReportHandler() = default;

  void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
    HTMLForm form(request, request.stream());
    
    try {
      if (HasInStr(request.getURI(), "/create_report")) {
        std::string login = form.get("login");
        std::string password = form.get("password");
        std::string product_name = form.get("product_name");

        std::optional<Report> report = ReportService::create_report(login, password, product_name);
        
        if (report.has_value()) {
          response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
          response.setChunkedTransferEncoding(true);
          response.setContentType("application/json");
          std::ostream &ostr = response.send();
          Poco::JSON::Stringifier::stringify(Report::to_json(report.value()), ostr);
          return;
        } else {
          response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
          response.setChunkedTransferEncoding(true);
          response.setContentType("application/json");
          Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
          root->set("type", "errors/failed_report");
          root->set("title", "Internal exception");
          root->set("status", "404");
          root->set("detail", "failed to create report");
          std::ostream &ostr = response.send();
          Poco::JSON::Stringifier::stringify(root, ostr);
          return;
        }
        
      }
    } catch(const std::exception& e) {
      std::cout << e.what() << std::endl;
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "errors/exception");
    root->set("title", "Page exception");
    root->set("status", "404");
    root->set("detail", "request not found");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
    
  }
};