#pragma once

#include <optional>

#include "../database/report.hpp"

class ReportService {
 public:
  ReportService() = delete;

  static std::optional<Report> ReportService::create_report(
    const std::string& login, 
    const std::string& passord,
    const std::string& tovar_name
  );
};