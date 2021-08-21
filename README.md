# shellLink

This is a project I decided to do the summer og 2021. I did not have a job and found network programming very fascinating. So I thought this would be a good use of my time. I also wanted to give C/C++ a try, I briefly used it on some assignments earlier that year. I will mostly program in these lower level languages (C/C++ is still regarded high level languages) in the future, I relly like the amount of control you get when working so close to the hardware.

ShellLink is a program that connects to computers terminals together directly, peer-to-peer. This is accomplished using a Kerberos inspired server to authenticate the two peers, and share the necessary metadata (gathered from a STUN-server) between the two peers. Then the peers can have a nice texted based chat.

While standing in the src diretory run the command: g++ main.cpp -std=c++17 -o main -ledit -pthreads && g++ LobbyServer/startAS.cpp -std=c++17 -o LobbyServer/startAS && g++ LobbyServer/startTGS.cpp -std=c++17 -o LobbyServer/startTGS to compile the code.

The src/enviorment.h file need to be updated, add the full path to the shellLink folderen the ENV_PATH.
