#include "server/http_server.hpp"

int main(int argc, char* argv[]) {
  HTTPUserServer app;
  return app.run(argc, argv);
}