#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <exception>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Parser.h>
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Util/ServerApplication.h"


using Poco::Data::Keywords::use;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::range;

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::ServerSocket;
using Poco::Util::ServerApplication;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTMLForm;


bool HasInStr(const std::string& str, const std::string& substr) {
  return str.find(substr) != std::string::npos;
}

struct Config {
  static const std::string host;
  static const std::string port;
  static const std::string login;
  static const std::string password;
  static const std::string database;
};

const std::string Config::host = "host";
const std::string Config::port = "3363";
const std::string Config::login = "root";
const std::string Config::password = "root";
const std::string Config::database = "mysql";

class Database {
 public:
  static Database& get() {
    static Database instance;
    return instance;
  };

  Poco::Data::Session create_session() {
    return Poco::Data::Session(conn_pool->get());
  };
 
 private:
  std::unique_ptr<Poco::Data::SessionPool> conn_pool;
  
  Database() {
    std::string conn_string = "";
    
    conn_string += "host=" + Config::host;
    conn_string += ";user=" + Config::login;
    conn_string += ";db=" + Config::database;
    conn_string += ";port=" + Config::port;
    conn_string += ";password=" + Config::password;

    std::cout << "Connection string:" << conn_string << std::endl;
    
    Poco::Data::MySQL::Connector::registerConnector();
    conn_pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::MySQL::Connector::KEY, conn_string);
  };
};


struct Report {
  int id;
  std::string srok;
  int id_ruk;
  int id_srok;
  int id_tovar;
  
  void save() {
    try {
      Poco::Data::Session session = Database::get().create_session();
      Poco::Data::Statement insert(session);
      
      insert << "INSERT INTO otchet (srok_godnosti, id_rukovoditela, id_sroka_godnosti, id_tovara) VALUES(?, ?, ?, ?)",
        use(srok),
        use(id_ruk),
        use(id_srok),
        use(id_tovar);

      insert.execute();
    }
    catch (...) {
      std::cout << "failed to insert report" << std::endl;
    }
  };

  static std::optional<Report> load(int id) { 
    try {
      Poco::Data::Session session = Database::get().create_session();
      Poco::Data::Statement select(session);
      Report report;
      select << "SELECT id_otcheta, srok_godnosti, id_rukovoditela, id_sroka_godnosti, id_tovara FROM otchet where id=?",
          into(report.id),
          into(report.srok),
          into(report.id_ruk),
          into(report.id_srok),
          into(report.id_tovar),
          use(id),
          range(0, 1);

      select.execute();
      Poco::Data::RecordSet rs(select);
      if (rs.moveFirst()) {
        return report; 
      }
    } catch (...) {
      std::cout << "failed to load report" << std::endl;
    }
    return {}; 
  };

  static Poco::JSON::Object::Ptr to_json(const Report& report) {
    Poco::JSON::Object::Ptr json_report = new Poco::JSON::Object();

    json_report->set("id_otcheta", report.id);
    json_report->set("srok_godnosti", report.srok);
    json_report->set("id_rukovoditela", report.id_ruk);
    json_report->set("id_sroka_godnosti", report.id_srok);
    json_report->set("id_tovara", report.id_tovar);

    return json_report;
  };
  
  static void create_report_table() {
    Poco::Data::Session session = Database::get().create_session();
    Poco::Data::Statement create_stmt(session);
    create_stmt << "CREATE TABLE IF NOT EXISTS `otchet`("
                << "`id_otcheta` INT NOT NULL AUTO_INCREMENT,"
                << "`srok_godnosti` VARCHAR(10) NOT NULL,"
                << "`id_rukovoditela` INT NOT NULL,"
                << "`id_sroka_godnosti` INT NOT NULL,"
                << "`id_tovara` INT NOT NULL,"
                << "PRIMARY KEY (`id`));",
                Poco::Data::Keywords::now;
  };
};

class ReportService {
 public:
  ReportService() = delete;

  static std::optional<Report> create_report(
    const std::string& login, 
    const std::string& password,
    const std::string& tovar_name
  ) {
    Report report;
    
    try {
      Poco::Data::Session session = Database::get().create_session();

      std::string select_login = login;
      std::string select_password = password;
      std::string select_tovar = tovar_name;

      int ruk_id = 0;
      Poco::Data::Statement ruk_select(session);
      ruk_select << "SELECT id_rukovoditela FROM rukovoditel where login=? and password=?",
        into(ruk_id),
        use(select_login),
        use(select_password),
        range(0, 1);
      ruk_select.execute();

      Poco::Data::RecordSet ruk_rs(ruk_select);
      if (!ruk_rs.moveFirst()) {
        return {}; 
      }

      int tovar_id = 0;
      int stroka_godnosti_id = 0;
      Poco::Data::Statement tovar_select(session);
      tovar_select << "SELECT id_tovara, id_stroka_godnosti FROM tovar where name=?",
        into(tovar_id),
        into(stroka_godnosti_id),
        use(select_tovar),
        range(0, 1);
      tovar_select.execute();

      Poco::Data::RecordSet tovar_rs(tovar_select);
      if (!tovar_rs.moveFirst()) {
        return {}; 
      }

      std::string goden = "";
      Poco::Data::Statement srok_select(session);
      srok_select << "SELECT goden FROM srok_godnosti where id_stroka_godnosti=?",
        into(goden),
        use(stroka_godnosti_id),
        range(0, 1);
      srok_select.execute();

      Poco::Data::RecordSet srok_rs(srok_select);
      if (!srok_rs.moveFirst()) {
        return {}; 
      }

      Report report;
      report.id_ruk = ruk_id;
      report.srok = goden;
      report.id_srok = stroka_godnosti_id;
      report.id_tovar = tovar_id;

      report.save();

      int report_id = 0;
      Poco::Data::Statement id_select(session);
      id_select << "SELECT LAST_INSERT_ID()",
        into(report_id),
        range(0, 1);

      id_select.execute();

      Poco::Data::RecordSet id_rs(id_select);
      if (!id_rs.moveFirst()) {
        return {}; 
      }

      return Report::load(report_id);

    } catch (Poco::Data::MySQL::StatementException &e) {
      std::cout << "statement exception: " << e.what() << std::endl;        
    } catch (Poco::Data::MySQL::ConnectionException &e) {
      std::cout << "connection exception: " << e.what() << std::endl;
    }

    return std::make_optional<Report>(report);
  };
};

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

class HTTPRequestFactory : public HTTPRequestHandlerFactory {
 public:
  HTTPRequestFactory() = default;

  HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) {
    return new ReportHandler();
  }
};

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