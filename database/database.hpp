#pragma once

#include <string>
#include <memory>

#include <Poco/Data/SessionPool.h>

class Database {
 public:
  static Database& get();
  Poco::Data::Session create_session();
 
 private:
  std::unique_ptr<Poco::Data::SessionPool> conn_pool;
  
  Database();
};