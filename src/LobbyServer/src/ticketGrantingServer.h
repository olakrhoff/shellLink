#ifndef TICKET_GRANTING_SERVER_H
#define TICKET_GRANTING_SERVER_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "types/tgs_req.h"
#include "types/krb_error.h"
#include "types/NetworkByteArray.h"
#include "types/stun_info.h"

class TicketGrantingServer
{
    private:
        char *ip = (char *)"192.168.0.125";
        unsigned short port {1251};
        sockaddr_in address;
        int socket_FD, temp_socket, addrlen {sizeof(this->address)}, number_of_users {}, users_size {2};
        char recive_buffer[4096] {}, send_buffer[4096] {};
        char **users;
        krb::STUN_INFO *user_stun_info;

    public:
        TicketGrantingServer();
        ~TicketGrantingServer();

        void start();

    private:
        void createSocket();
        void bindSocket();
        void listenToSocket();
        bool acceptSocket();
        void handleClient();
        void handleMessage(krb::NetworkByteArray &binaryDataBuffer);
        void handleRequest(krb::NetworkByteArray &binaryDataBuffer);
        int doesUserExist(krb::PrincipalName &&username);
        void sendRep(krb::TGS_REQ &request);
        void sendError(krb::KRB_ERROR &error);
        void addSTUNInfoToUser(krb::STUN_INFO &stun, int user_index);
        krb::STUN_INFO *checkUserSTUN(int user_index);
};

#endif //TICKET_GRANTING_SERVER_H