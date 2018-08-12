#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <string>
#include <map>

namespace rpc {

template <typename T>
std::string compServiceName(T arg)
{
    return arg;
}

template <typename T, typename... Types>
std::string compServiceName(T first, Types... rest)
{
    return first + "." + compServiceName(rest...);
}

template <class IService>
struct ServiceFactory
{
    virtual IService *newService() = 0;
    virtual const std::string &getName() = 0;
};

template <class IService, class TService>
class ServiceFactoryImpl : public ServiceFactory<IService>
{
  public:
    ServiceFactoryImpl(const std::string &serviceName) : mServiceName(serviceName) {}

    virtual IService *newService()
    {
        TService *svr = new TService();
        return svr;
    }

    virtual const std::string &getName()
    {
        return mServiceName;
    }

  private:
    std::string mServiceName;
};

template<class IService>
class ServiceFactoryRegistry
{
  public:
    typedef ServiceFactory<IService> IFactory;
    typedef std::map<std::string, IFactory *> FactoryMap;

    ServiceFactoryRegistry() { mbValid = true; }
    virtual ~ServiceFactoryRegistry() { mbValid = false; }

    static ServiceFactoryRegistry *getRegistry()
    {
        static ServiceFactoryRegistry s_registry;
        return &s_registry;
    }
    static bool isValid() { return mbValid; }

    void registerFactory(IFactory *f)
    {
        mFactories.insert(std::make_pair(f->getName(), f));
    }
    void unregisterFactory(IFactory *f)
    {
        typename FactoryMap::iterator i = mFactories.find(f->getName());
        if (i != mFactories.end())
        {
            mFactories.erase(i);
        }
    }

    IFactory *getServiceFactory(const std::string &serviceName)
    {
        typename FactoryMap::iterator i = mFactories.find(serviceName);
        if (i != mFactories.end())
        {
            return i->second;
        }
        return NULL;
    }
    IFactory *getServiceFactory(const std::string &n1, const std::string &n2)
    {
        std::string serviceName = compServiceName(n1, n2);
        return getServiceFactory(serviceName);
    }
    IFactory *getServiceFactory(const std::string &n1, const std::string &n2, const std::string &n3)
    {
        std::string serviceName = compServiceName(n1, n2, n3);
        return getServiceFactory(serviceName);
    }
    IFactory *getServiceFactory(const std::string &n1, const std::string &n2, const std::string &n3, const std::string &n4)
    {
        std::string serviceName = compServiceName(n1, n2, n3, n4);
        return getServiceFactory(serviceName);
    }

  protected:
    FactoryMap mFactories;
    static bool mbValid;
};

template <class IService, class TService>
class AutoRegisterService
{
  public:
    AutoRegisterService(const std::string &serviceName)
            :mRegistry(ServiceFactoryRegistry<IService>::getRegistry())
            ,mFactory(serviceName)
    {
        mRegistry->registerFactory(&mFactory);
    }

    virtual ~AutoRegisterService()
    {
        if (ServiceFactoryRegistry<IService>::isValid())
        {
            mRegistry->unregisterFactory(&mFactory);
        }
    }

  private:
    ServiceFactoryRegistry<IService> *mRegistry;
    ServiceFactoryImpl<IService, TService> mFactory;
};

#define REG_SERVICE(IService, TService, arg...)                         \
    static rpc::AutoRegisterService<IService, TService> _autoRegisterService_##TService( \
        rpc::compServiceName(arg))

} // rpc

#endif // __SERVICE_H__
