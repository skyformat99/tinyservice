#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/concurrency/Monitor.h>
#include <thrift/concurrency/Util.h>
#include <thrift/concurrency/Mutex.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/processor/TMultiplexedProcessor.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TFileTransport.h>
#include <thrift/TLogging.h>
#include <thrift/stdcxx.h>

#include <iostream>
#include <set>
#include <stdexcept>
#include <sstream>
#include <map>

#include <boost/smart_ptr.hpp>

#include "rpc.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

namespace rpc {

bool ProcessorFactoryRegistry::mbValid = false;

ProcessorFactoryRegistry::ProcessorFactoryRegistry()
{
    mbValid = true;
}

ProcessorFactoryRegistry::~ProcessorFactoryRegistry()
{
    mbValid = false;
}

ProcessorFactoryRegistry *ProcessorFactoryRegistry::getRegistry()
{
    static ProcessorFactoryRegistry s_registry;
    return &s_registry;
}

bool ProcessorFactoryRegistry::isValid()
{
    return mbValid;
}

void ProcessorFactoryRegistry::registerFactory(ProcessorFactory *factory)
{
    mFactories.push_back(factory);
}

void ProcessorFactoryRegistry::unregisterFactory(ProcessorFactory *factory)
{
    ProcessorFactoryList::iterator it = mFactories.begin();
    while (it != mFactories.end())
    {
        if ((*it) == factory)
        {
            it = mFactories.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ProcessorFactoryRegistry::all(void (*doit)(ProcessorFactory *f, void *ctx), void *ctx)
{
    ProcessorFactoryList::iterator it = mFactories.begin();
    for (; it != mFactories.end(); it++)
    {
        doit(*it, ctx);
    }
}

static void registerProcessor(rpc::ProcessorFactory *f, void *ctx)
{
    TMultiplexedProcessor *multiProcessor = (TMultiplexedProcessor *)ctx;
    multiProcessor->registerProcessor(f->getName(), f->newProcessor());
}

void rpcLoop(int port, int workerCount, int pendingTaskCountMax)
{
    // Processor
    stdcxx::shared_ptr<TMultiplexedProcessor> multiProcessor(new TMultiplexedProcessor());
    rpc::ProcessorFactoryRegistry::getRegistry()->all(registerProcessor, multiProcessor.get());

    // Protocol Factory
    stdcxx::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    // Thread Pool
    stdcxx::shared_ptr<ThreadManager> threadManager(ThreadManager::newSimpleThreadManager(workerCount, pendingTaskCountMax));
    // stdcxx::shared_ptr<PosixThreadFactory> threadFactory(new PosixThreadFactory());
    stdcxx::shared_ptr<PlatformThreadFactory> threadFactory(new PlatformThreadFactory());

    threadManager->threadFactory(threadFactory);
    threadManager->start();

    // Nonblocking Service
    stdcxx::shared_ptr<TNonblockingServerSocket> nbSocket(new TNonblockingServerSocket(port));

    stdcxx::shared_ptr<TNonblockingServer> server(new TNonblockingServer(multiProcessor, protocolFactory, nbSocket, threadManager));

    server->serve();
}

} // rpc
