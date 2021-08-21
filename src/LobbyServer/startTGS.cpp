//g++ LobbyServer/startTGS.cpp -std=c++17 -o LobbyServer/startTGS

#include <iostream>

#include "src/ticketGrantingServer.cpp"

int main()
{
    TicketGrantingServer tgs;
    
    tgs.start();

    return 0;
}