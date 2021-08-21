//g++ LobbyServer/startAS.cpp -std=c++17 -o LobbyServer/startAS

#include <iostream>

#include "src/authenticationServer.cpp"

int main()
{
    AuthenticationServer as;
    
    as.start();

    return 0;
}