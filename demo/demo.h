#ifndef __DEMO_H__
#define __DEMO_H__

#include "src/rpc.h"
#include "src/service.h"

#define DEMO_MODULE "demo"
#define REG_DEMO_SERVICE(IService, TService, ...)                       \
    REG_SERVICE(IService, TService, std::string(DEMO_MODULE), __VA_ARGS__)

using namespace rapidjson;

namespace demo {

}

#endif // __DEMO_H__
