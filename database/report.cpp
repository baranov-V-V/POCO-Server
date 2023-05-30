#include <sstream>
#include <exception>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include "report.hpp"
#include "database.hpp"

std::optional<Report> Report::load(int id) { return std::optional<Report>(); }

//later
Poco::JSON::Object::Ptr Report::to_json(const Report& report) {
  Poco::JSON::Object::Ptr json_report = new Poco::JSON::Object();

  json_report->set("id", _id);
  json_report->set("name", _name);
  json_report->set("weight", _weight);
  json_report->set("price", _price);
  json_report->set("login", _login);

  return json_report;
}

void Report::crate_report_table() {
  Poco::Data::Session session = Database::get().create_session();
  Poco::Data::Statement create_stmt(session);
  create_stmt << "CREATE TABLE IF NOT EXISTS `otchet` (`id` INT NOT NULL AUTO_INCREMENT,"
              << "`name` VARCHAR(256) NOT NULL,"
              << "`weight` VARCHAR(256) NOT NULL,"
              << "`price` VARCHAR(256) NOT NULL,"
              << "`login` VARCHAR(256) NOT NULL,"
              << "PRIMARY KEY (`id`), KEY `lo` (`login`));",
              Poco::Data::Keywords::now;
}

void Report::save() {
  try {
    Poco::Data::Session session = Database::get().create_session();
    Poco::Data::Statement insert(session);
    //rework
    insert << "INSERT INTO otchet (name,weight,price,login) VALUES(?, ?, ?, ?)",
        use(name),
        use(weight),
        use(price),
        use(login),
    insert.execute();
  }
  catch (...) {
    std::cout << "failed to insert report" << std::endl;
  }
}
