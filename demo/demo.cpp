#include "src/json_service.h"
#include "gen-cpp/Demo.h"
#include "demo.h"

using namespace rapidjson;

namespace demo {

using rpc::json::Service;

class EchoService : public Service
{
  public:
    EchoService() {}
    virtual ~EchoService() {}

    virtual void run(const Document &indom, Document &outdom)
    {
        outdom.AddMember("code", Value().SetInt(0), outdom.GetAllocator());
        outdom.AddMember("errmsg", Value("Hello, I am Echo"), outdom.GetAllocator());
    }
};

class SayHiService : public Service
{
  public:
    SayHiService() {}
    virtual ~SayHiService() {}

    virtual void run(const Document &indom, Document &outdom)
    {
        outdom.AddMember("code", Value().SetInt(0), outdom.GetAllocator());
        outdom.AddMember("errmsg", Value("Hi, How are you!"), outdom.GetAllocator());
    }
};

REG_DEMO_SERVICE(Service, EchoService, "echo");
REG_DEMO_SERVICE(Service, SayHiService, "sayhi");

class Demo : public DemoIf
{
  public:
    Demo() {}
    virtual ~Demo() {}

    virtual void rpcCall(std::string& _return, const std::string& jsonarg)
    {
        rpc::json::ServiceProxy::getInstance()->rpcCall(_return, jsonarg, DEMO_MODULE);
    }
};

PROCESSOR_REGISTER("Demo", DemoProcessor, Demo);

}
