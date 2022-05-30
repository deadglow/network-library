#include "SNET_Client.h"
#include <iostream>

SNET_Client::~SNET_Client()
{
    if (connected)
        Disconnect();

    if (host != nullptr)
        enet_host_destroy(host);

    enet_deinitialize();
}

int SNET_Client::Initialise()
{
    if (enet_initialize() != 0)
        return EXIT_FAILURE;

    std::cout << "ENet initialised.\n";

    host = enet_host_create(nullptr, 1, SNET_CHANNEL_COUNT, 0, 0);

}
