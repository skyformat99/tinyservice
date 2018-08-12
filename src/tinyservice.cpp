#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "rpc.h"
#include "service.h"

using namespace rpc;

void useage()
{
    fprintf(stdout, "no useage right now!\n");
}

int main(int argc, char *argv[])
{
    int rpcPort = 8888, workerCount = 32, pendingTaskCountMax = 0;
    int opt = 0;

    while ((opt = getopt(argc, argv, "p:t:T:h")) != -1)
    {
        switch (opt)
        {
        case 'p':
            rpcPort = atoi(optarg);
            break;
        case 't':
            workerCount = atoi(optarg);
            break;
        case 'T':
            pendingTaskCountMax = atoi(optarg);
            break;
        case 'h':
        case '?':
        default:
            useage();
            exit(1);
        }
    }

    rpc::rpcLoop(rpcPort, workerCount, pendingTaskCountMax);

    exit(0);
}
