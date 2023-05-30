#include "database.hpp"
#include "config/config.hpp"

Database::Database(){
  std::string conn_string = "";
  
  conn_string += "host=" + Config::host;
  conn_string += ";user=" + Config::login;
  conn_string += ";db=" + Config::database;
  conn_string += ";port=" + Config::port;
  conn_string += ";password=" + Config::password;

  std::cout << "Connection string:" << conn_string << std::endl;
  
  Poco::Data::MySQL::Connector::registerConnector();
  conn_pool = std::make_unique<Poco::Data::SessionPool>(Poco::Data::MySQL::Connector::KEY, conn_string);
}

Database& Database::get(){
  static Database instance;
  return instance;
}

Poco::Data::Session Database::create_session(){
  return Poco::Data::Session(conn_pool->get());
}