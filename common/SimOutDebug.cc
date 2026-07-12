#include "SimOutDebug.h"
#include "AsyncOutDebug.h"
#include <thread>

void SimOutDebug::init()
{  
  auto log_frontend = new AsyncOut::Debug();

  Tools::x_debug = log_frontend;

  std::thread([](auto log_frontend) {
    AsyncOut::Logger backend;
    log_frontend->subscribe(&backend);

    AsyncOut::FileLogger file_backend{};
    log_frontend->subscribe(&file_backend);
    

    while (true) {
      backend.log();
      file_backend.log();
      backend.wait();
      //std::this_thread::sleep_for(200ms);
    }
  }, log_frontend).detach();  
}
