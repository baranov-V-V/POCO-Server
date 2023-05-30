#include <sstream>
#include <exception>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Parser.h>

#include "report.hpp"
#include "database.hpp"

using Poco::Data::Keywords::use;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::range;

std::optional<Report> Report::load(int id) { 
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
}

Poco::JSON::Object::Ptr Report::to_json(const Report& report) {
  Poco::JSON::Object::Ptr json_report = new Poco::JSON::Object();

  json_report->set("id_otcheta", report.id);
  json_report->set("srok_godnosti", report.srok);
  json_report->set("id_rukovoditela", report.id_ruk);
  json_report->set("id_sroka_godnosti", report.id_srok);
  json_report->set("id_tovara", report.id_tovar);

  return json_report;
}

void Report::create_report_table() {
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
}

void Report::save() {
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
}
