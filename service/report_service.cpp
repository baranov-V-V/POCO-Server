#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Parser.h>

#include "report_service.hpp"
#include "../database/database.hpp"

using Poco::Data::Keywords::use;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::range;

std::optional<Report> ReportService::create_report(
    const std::string& login, const std::string& password,
    const std::string& tovar_name) {
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
}
