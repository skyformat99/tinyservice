#ifndef __RPC_H__
#define __RPC_H__

#include <vector>
#include <thrift/stdcxx.h>
#include <thrift/TProcessor.h>

using namespace apache::thrift;

namespace rpc {

struct ProcessorFactory
{
    virtual stdcxx::shared_ptr<TProcessor> newProcessor() = 0;
    virtual const std::string &getName() const = 0;
};

template <class Processor, class Handler>
class ProcessorFactoryImpl : public ProcessorFactory
{
  public:
    ProcessorFactoryImpl(const std::string &name) : mName(name) {}

    virtual stdcxx::shared_ptr<TProcessor> newProcessor()
    {
        return stdcxx::shared_ptr<Processor>(new Processor(stdcxx::make_shared<Handler>()));
    }

    virtual const std::string &getName() const
    {
        return mName;
    }

  private:
    std::string mName;
};

typedef std::vector<ProcessorFactory *> ProcessorFactoryList;
class ProcessorFactoryRegistry
{
  public:
    ProcessorFactoryRegistry();
    ~ProcessorFactoryRegistry();

    static ProcessorFactoryRegistry *getRegistry();
    static bool isValid();

    void registerFactory(ProcessorFactory *factory);
    void unregisterFactory(ProcessorFactory *factory);

    void all(void (*doit)(ProcessorFactory *f, void *ctx), void *ctx);

  protected:
    ProcessorFactoryList mFactories;
    static bool mbValid;
};

template <class Processor, class Handler>
class AutoRegisterProcessor
{
  public:
    AutoRegisterProcessor(const std::string &name)
            :mRegistry(ProcessorFactoryRegistry::getRegistry())
            ,mFactory(name)
    {
        mRegistry->registerFactory(&mFactory);
    }

    ~AutoRegisterProcessor()
    {
        if (ProcessorFactoryRegistry::isValid())
        {
            mRegistry->unregisterFactory(&mFactory);
        }
    }

  private:
    ProcessorFactoryRegistry *mRegistry;
    ProcessorFactoryImpl<Processor, Handler> mFactory;
};

void rpcLoop(int port, int workerCount, int pendingTaskCountMax);

} // rpc

#define PROCESSOR_REGISTER(Name, Processor, Handler)                    \
    static rpc::AutoRegisterProcessor<Processor, Handler> _autoRegisterRegistry_##Handler(Name)

#endif // __RPC_H__
