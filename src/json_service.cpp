#include <thrift/stdcxx.h>
#include "service.h"
#include "json_service.h"

using namespace apache::thrift;
using namespace rapidjson;

template <>
bool rpc::ServiceFactoryRegistry<rpc::json::Service>::mbValid = false;

namespace rpc {
namespace json {

typedef rpc::ServiceFactory<rpc::json::Service> JsonServiceFactory;
typedef rpc::ServiceFactoryRegistry<rpc::json::Service> JsonServiceRegistry;

ServiceProxy *ServiceProxy::getInstance()
{
    static ServiceProxy s_proxy;
    return &s_proxy;
}

void ServiceProxy::rpcCall(std::string &_return, const std::string &jsonarg, const std::string &module)
{
    Document indom;
    Document outdom(kObjectType);
    if (indom.Parse(jsonarg.c_str()).HasParseError())
    {
        outdom.AddMember("code", Value().SetInt(1), outdom.GetAllocator());
        outdom.AddMember("errmsg", Value("invalid json args"), outdom.GetAllocator());
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);
        outdom.Accept(writer);
        _return = sb.GetString();
        return;
    }
    if (!indom.HasMember("action"))
    {
        outdom.AddMember("code", Value().SetInt(1), outdom.GetAllocator());
        outdom.AddMember("errmsg", Value("no action specified"), outdom.GetAllocator());
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);
        outdom.Accept(writer);
        _return = sb.GetString();
        return;
    }
    if (!indom["action"].IsString())
    {
        outdom.AddMember("code", Value().SetInt(1), outdom.GetAllocator());
        outdom.AddMember("errmsg", Value("invalid action type"), outdom.GetAllocator());
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);
        outdom.Accept(writer);
        _return = sb.GetString();
        return;
    }

    JsonServiceFactory *f =
            JsonServiceRegistry::getRegistry()->getServiceFactory(module, indom["action"].GetString());
    if (!f)
    {
        outdom.AddMember("code", Value().SetInt(1), outdom.GetAllocator());
        outdom.AddMember("errmsg", Value("no registed action"), outdom.GetAllocator());
        StringBuffer sb;
        PrettyWriter<StringBuffer> writer(sb);
        outdom.Accept(writer);
        _return = sb.GetString();
        return;
    }

    stdcxx::shared_ptr<Service> s(f->newService());
    s->run(indom, outdom);
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    outdom.Accept(writer);
    _return = sb.GetString();
}

}
}
