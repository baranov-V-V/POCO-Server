#pragma once

#include <iostream>
#include <iostream>
#include <fstream>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
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

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Net::HTMLForm;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;

#include "../../database/report.hpp"

class ReportHandler : public HTTPRequestHandler {
 public:
  ReportHandler(const std::string &format) : format(format) {}

  void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
    HTMLForm form(request, request.stream());
    if (hasSubstr(request.getURI(), "/user_by_id"))
    {
        long id = atol(form.get("id").c_str());

        std::optional<database::User> result = database::User::read_by_id(id);
        if (result)
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setChunkedTransferEncoding(true);
            response.setContentType("application/json");
            std::ostream &ostr = response.send();
            Poco::JSON::Stringifier::stringify(remove_password(result->toJSON()), ostr);
            return;
        }
        else
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
            response.setChunkedTransferEncoding(true);
            response.setContentType("application/json");
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("type", "/errors/not_found");
            root->set("title", "Internal exception");
            root->set("status", "404");
            root->set("detail", "user ot found");
            root->set("instance", "/user");
            std::ostream &ostr = response.send();
            Poco::JSON::Stringifier::stringify(root, ostr);
            return;
        }
    }
        else
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            std::ostream &ostr = response.send();
            ostr << message;
            response.send();
            return;
        }
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/not_found");
    root->set("title", "Internal exception");
    root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    root->set("detail", "request ot found");
    root->set("instance", "/user");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
  };
  
 private:
  std::string format;
};