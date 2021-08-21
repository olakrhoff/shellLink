#ifndef AUTHENTICATION_SERVER_H
#define AUTHENTICATION_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#include "types/as_req.h"
#include "types/krb_error.h"
#include "types/NetworkByteArray.h"

class AuthenticationServer
{
    private:
        char *ip = (char *)"0.0.0.0";
        unsigned short port {1250};
        sockaddr_in address;
        int socket_FD, temp_socket, addrlen {sizeof(address)}, number_of_users {};
        char recive_buffer[4096] {}, send_buffer[4096] {};
        char **users;

    public:
        AuthenticationServer();
        ~AuthenticationServer();

        void start();

    private:
        void createSocket();
        void bindSocket();
        void listenToSocket();
        bool acceptSocket();
        void handleClient();
        void handleMessage(krb::NetworkByteArray &binaryDataBuffer);
        void handleRequest(krb::NetworkByteArray &binaryDataBuffer);
        bool doesUserExist(krb::PrincipalName &&username);
        bool doesServerExist(krb::PrincipalName &&server_name);
        void sendRep(krb::AS_REQ &request);
        void sendError(krb::KRB_ERROR &error);


};

#endif //AUTHENTICATION_SERVER_H