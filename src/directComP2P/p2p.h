#ifndef P2P_H
#define P2P_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <string>

#include "../LobbyServer/src/types/NetworkByteArray.h"
#include "../LobbyServer/src/types/PrincipalName.h"
#include "../LobbyServer/src/types/Ticket.h"
#include "../LobbyServer/src/types/as_rep.h"
#include "../LobbyServer/src/types/stun_info.h"

class P2P
{
    private:
        sockaddr_in address;
        int socketFileDescripter, incoming_socket, addrlen = sizeof(address);
        bool running {false};
        char recive_buffer[4096] {}, send_buffer[4096] {};
        std::thread reciveThread, sendThread;
        char *peer_name {nullptr};
        krb::STUN_INFO stun_info;

    public:
        P2P();
        P2P(const P2P &other);
        ~P2P();
        void start();

    private:
        void listenToClient();
        void sendToClient();
        void shutdownConnection();
        void setupHosting();
        void setupConnecting();
        void sendASRequest(krb::PrincipalName &&client_name);
        void listenForReply();
        void handleReply(krb::NetworkByteArray &binaryDataBuffer);
        void handleError(krb::NetworkByteArray &binaryDataBuffer);
        void handleASRep(krb::NetworkByteArray &binaryDataBuffer);
        void sendTGSRequest(krb::AS_REP &rep);
        void handleTGSRep(krb::NetworkByteArray &binaryDataBuffer);
        void startSTUN();
};

#endif //P2P_H