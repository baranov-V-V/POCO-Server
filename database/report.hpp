#pragma once

#include <string>
#include <vector>
#include <optional>

#include "Poco/JSON/Object.h"

struct Report {
  int id;
  std::string srok;
  int id_ruk;
  int id_srok;
  int id_tovar;
  
  void save();
  static std::optional<Report> load(int id);

  static Poco::JSON::Object::Ptr to_json(const Report& report);
  
  static void create_report_table();
};