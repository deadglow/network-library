#include "enet/enet.h"
#include <iostream>

#define IS_HOST false

void Host()
{

}

void Client()
{

}

int main()
{
    if (enet_initialize() != 0)
    {
        std::cout << "An error occured while initializing ENet." << std::endl;
        return EXIT_FAILURE;
    }
    else
        std::cout << "ENet initialized." << std::endl;

    if (IS_HOST)
    {
        Host();
    }
    else
    {
        Client();
    }

    enet_deinitialize();
    return 0;
}