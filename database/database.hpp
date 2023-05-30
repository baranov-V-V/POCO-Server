#pragma once

#include <string>
#include <memory>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SessionPool.h>

class Database{
 public:
  
  static Database& get();
  Poco::Data::Session create_session();
 
 private:
  std::string conn_string;
  std::unique_ptr<Poco::Data::SessionPool> conn_pool;
  
  Database();
};