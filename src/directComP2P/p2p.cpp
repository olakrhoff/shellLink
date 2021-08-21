#include "p2p.h"

#include <iostream>
#include <string.h>
#include <sstream>
#include <arpa/inet.h>
#include <fstream>

#include "../enviorment.h"
#include "../LobbyServer/src/types/types.h"
#include "../LobbyServer/src/types/krb_error.h"
#include "../LobbyServer/src/types/as_req.h"
#include "../LobbyServer/src/types/as_rep.h"
#include "../LobbyServer/src/types/tgs_req.h"
#include "../LobbyServer/src/types/Ticket.h"
#include "../LobbyServer/src/types/stun_message.h"
#include "../LobbyServer/src/types/stun_message.h"
#include "../LobbyServer/src/types/stun_attributes/xor_mapped_address.h"

//Authentication-Server
#define AS_IP "0.0.0.0"
#define AS_PORT 1250

//Ticket-Granting-Server
#define TGS_IP "192.168.0.125"
#define TGS_PORT 1251

//stun.l.google.com:19302
#define STUN_IP "64.233.163.127"
//#define STUN_IP "84.208.20.110"
#define STUN_PORT 19302
//#define STUN_PORT 53

#define MAX_USERNAME_LENGTH 32

P2P::P2P()
{
};

P2P::P2P(const P2P &other) {};

P2P::~P2P()
{
    if (this->peer_name != nullptr)
        free(this->peer_name);
    if (this->running)
        this->shutdownConnection();
};

void P2P::start()
{
    if (this->running)
        return; //The program is already running 
    this->running = true;
    this->startSTUN();
    char choice[5];
    while (true)
    {
        memset(choice, 0, 5);
        printf("Do you want to connect(c) or host(h), c/h? -- 'exit' to exit.\n");
        
        while (true)
        {
            fgets(choice, 5, stdin);
            if (strncmp(choice, "\n", 2) != 0)
                break;
        }

        if (strncmp(choice, "c", 1) == 0 && strlen(choice) == 2) //Connect to a socket
        {
            printf("Connecting...\n");
            this->setupConnecting();
        }
        else if (strncmp(choice, "h", 1) == 0 && strlen(choice) == 2) //Setup a socket that someone can connect to
        {
            printf("Hosting...\n");
            this->setupHosting();
        }
        else if (strncmp(choice, "exit", 4) == 0 && strlen(choice) == 4)
            return;
        else
            continue;
        return;
    }
}

void P2P::listenToClient()
{
    this->reciveThread = std::thread([&]{
        while (this->running)
        {
            memset(this->recive_buffer, 0, sizeof(this->recive_buffer)); //Nulling out the memory
            int bytesReceived = recv(this->incoming_socket, this->recive_buffer, sizeof(this->recive_buffer), 0);
            
            if (!this->running) //In the case it was blocking on recv(), and running changed
                return;

            if (bytesReceived == -1)
            {
                printf("Error on recv()\n");
                return;
            }
            if (bytesReceived == 0)
            {
                printf("Other end of TCP-connection is closed\n");
                this->running = false;
                return;
            }

            printf("Client: %.*s\n", (int)strlen(this->recive_buffer), this->recive_buffer);
        }
    });
}

void P2P::sendToClient()
{
    this->sendThread = std::thread([&]{
        int messageLength {};
        while (this->running)
        {
            fgets(this->send_buffer, sizeof(this->send_buffer), stdin);

            if (strncmp(this->send_buffer, "exit", 4) == 0 && strlen(this->send_buffer) == 5)
            {
                printf("Exiting the connection\n");
                this->running = false;
                this->shutdownConnection();
                return;
            }
            messageLength = strlen(this->send_buffer);


            //DEBUG

            printf("Sent (%d): %.*s\n", messageLength, messageLength, this->send_buffer);
        
            printf("OBJ: '");
            for (int i = 0; i < messageLength; i++)
                printf("%x, ", (unsigned char)this->send_buffer[i]);
            printf("'\n");

            //DEBUG

            send(this->incoming_socket, this->send_buffer, messageLength, 0); 
            printf("\n");
            memset(this->send_buffer, 0, messageLength); //Nulling out the memory
        }
    });
}

void P2P::shutdownConnection()
{
    this->running = false;
    shutdown(this->socketFileDescripter, SHUT_RDWR);
    shutdown(this->incoming_socket, SHUT_RDWR);
}

void P2P::setupHosting()
{
    char buffer[MAX_USERNAME_LENGTH];
    int length;

    printf("What is your username?\n");
    fgets(buffer, sizeof(buffer), stdin);
    length = strlen(buffer);

    memset(&buffer[length], 0, MAX_USERNAME_LENGTH - length);
    buffer[length - 1] = 0; //Setting the last "extra" char to 0
    
    std::string clientUsername = buffer;

    
    printf("What is your password?\n");
    fgets(buffer, sizeof(buffer), stdin);
    length = strlen(buffer);

    memset(&buffer[length], 0, MAX_USERNAME_LENGTH - length);
    buffer[length - 1] = 0; //Setting the last "extra" char to 0

    std::string clientPassword = buffer;
    

    printf("Whom do you want to connect to?\n");
    fgets(buffer, sizeof(buffer), stdin);
    length = strlen(buffer);

    memset(&buffer[length], 0, MAX_USERNAME_LENGTH - length);
    buffer[length - 1] = 0; //Setting the last "extra" char to 0

    std::string peerUsername = buffer;

    this->peer_name = (char *)malloc(peerUsername.length());
    memcpy(this->peer_name, peerUsername.data(), peerUsername.length());

    memset(&buffer, 0, sizeof(buffer)); //Clearing of the whole buffer

    //Sendign AS_REQ
    this->sendASRequest(krb::PrincipalName(krb::PrincipalNameTypes::NT_PRINCIPAL, krb::KerberosString(clientUsername)));

    //Listening for answer from AS
    this->listenForReply();


/*
    return; //TEMP

    this->socketFileDescripter = socket(AF_INET, SOCK_STREAM, 0); //Ipv4, TCP

    if (this->socketFileDescripter == -1) //Error occured while making the socket
    {
        printf("Failed to create socket...\n");
        return;
    }

    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(8080); //Networking is BIG_ENDIAN and the machine is LITTLE_ENDIAN, hotns: HostTONetworkShort

    while (true)
    {
        if (bind(this->socketFileDescripter, (struct sockaddr *)&this->address, sizeof(this->address)) == -1)
        {
            printf("Failed to bind the address to the socket\nRetrying...\n");
            in_port_t newPort {};
            while (true)
            {
                newPort = rand();
                if (newPort > (in_port_t)1024)
                    break;
            }
            this->address.sin_port = htons(newPort);
            continue;
        }
        else
            break;
    }

    if (listen(this->socketFileDescripter, 1) == -1) //Setting the queue to 1
    {
        printf("Failed to listen to the socket\n");
        return;
    }
    
    //Showing socket-info to user
    const char* addr = inet_ntop(AF_INET, &this->address.sin_addr.s_addr, this->send_buffer, sizeof(this->send_buffer));

    printf("Settings: \nIPv4: %s\nPort: %hu\n", addr, ntohs(this->address.sin_port));

    printf("Connecting...\n");
    if ((this->incoming_socket = accept(this->socketFileDescripter, (struct sockaddr *)&this->address, (socklen_t*)&this->addrlen)) == -1)
    {
        printf("Failed to accept socket connection\n");
        return;
    }
    printf("Connected\n");

    shutdown(this->socketFileDescripter, SHUT_RDWR);

    memset(this->recive_buffer, 0, sizeof(this->recive_buffer)); //Nulling out the memory
    memset(this->send_buffer, 0, sizeof(this->send_buffer)); //Nulling out the memory

    printf("\n------CHAT-------\n\n"); //Spacing
    listenToClient();
    sendToClient();
    this->reciveThread.join();
    this->sendThread.join();
    this->shutdownConnection();

    */
}

void P2P::setupConnecting()
{
    if ((this->incoming_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error whilst creating socket\n");
        return;
    }

    char choice[15]; //15 is the max size of the IPv4 address
    memset(choice, 0, sizeof(choice)); //Setting the char array to zeroes
    printf("What is the IPv4 address? (format: 192.0.168.1)\n");
    fgets(choice, sizeof(choice), stdin);
    int length = strlen(choice);

    memset(&choice[length], 0, 15 - length);
    choice[length - 1] = 0; //Setting the last "extra" char to 0

    int check = inet_pton(AF_INET, choice, &this->address.sin_addr.s_addr);

    printf("What is the port number? (0 - 65535)\n");
    fgets(choice, sizeof(choice), stdin);
    in_port_t port = (short)atoi(choice);


    this->address.sin_family = AF_INET; //IPv4
    this->address.sin_port = htons(port);

    if(check <= 0) 
    {
        printf("Address not recognized or is invalid\n");
        return;
    }
   
    printf("Connecting...\n");
    if (connect(this->incoming_socket, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
    {
        printf("Error when connecting to the socket\n");
        return;
    }
    printf("Connected\n");

    memset(this->recive_buffer, 0, sizeof(this->recive_buffer)); //Nulling out the memory
    memset(this->send_buffer, 0, sizeof(this->send_buffer)); //Nulling out the memory


    printf("\n------CHAT-------\n\n"); //Spacing
    listenToClient();
    sendToClient();
    this->reciveThread.join();
    this->sendThread.join();
    this->shutdownConnection();
}

void P2P::sendASRequest(krb::PrincipalName &&client_name)
{
    krb::AS_REQ requestAS;
    requestAS.setClientName(client_name);
    requestAS.setServerName(krb::PrincipalName(krb::PrincipalNameTypes::NT_PRINCIPAL, krb::KerberosString("TGS")));

    if ((this->incoming_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error whilst creating socket to connect to AS.\n");
        return;
    }

    int check = inet_pton(AF_INET, AS_IP, &this->address.sin_addr.s_addr);

    this->address.sin_family = AF_INET; //IPv4
    this->address.sin_port = htons(AS_PORT);

    if(check <= 0) 
    {
        printf("Address not recognized or is invalid. Could not connect to Lobby-Server.\n");
        return;
    }
   
    //Connecting to the AS
    if (connect(this->incoming_socket, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
    {
        printf("Error when connecting to the socket\n");
        return;
    }

    //Connected to the AS
    printf("Connected\n");

    //Sending AS_REQ
    krb::NetworkByteArray obj;
    requestAS.serialize(obj);

    requestAS.print();
    
    printf("Sending..\n");

    unsigned char* data {nullptr};
    obj.getByteArray(data);

    send(this->incoming_socket, data, obj.getSize(), 0); //Sending the AS_REQ

    free(data);
}

void P2P::listenForReply()
{
    memset(this->recive_buffer, 0, sizeof(this->recive_buffer)); //Nulling out the memory
    
    int bytesReceived {-2};
    std::thread t([&](){
        bytesReceived = recv(this->incoming_socket, this->recive_buffer, sizeof(this->recive_buffer), 0);
    });
    
    //We wait for 10 seconds to get a response from the AS, if no response, the connection is considerd as failed
    bool timeout {false};
    auto time = std::chrono::system_clock::now();
    while (!t.joinable() && !timeout)
    {
        if (std::chrono::system_clock::now() - time > std::chrono::seconds(10))
            timeout = true;
    }

    if (timeout)
    {
        printf("No response from the AS\n");
        this->shutdownConnection();
        t.join();
        return;
    }
    t.join();

    if (bytesReceived == -1)
    {
        printf("Error on recv() rep");
        return;
    }
    if (bytesReceived == 0)
    {
        printf("Server end, TCP-connection is closed\n");
        return;
    }

    krb::NetworkByteArray replyBuffer = krb::NetworkByteArray((unsigned char*)this->recive_buffer, bytesReceived);
    this->handleReply(replyBuffer);
}

void P2P::handleReply(krb::NetworkByteArray &binaryDataBuffer)
{
    if (binaryDataBuffer[0] != PVNO) //Checking that the pvno (version number is correct)
    {
        printf("Reply in unsupported version: %d", binaryDataBuffer[0]);
        return;
    }
    
    switch (binaryDataBuffer[1]) //Checking the msg_type
    {
        case krb::enums::KerberosMessageTypes::AS_REP:
            printf("Classified as: AS_REP\n");
            this->handleASRep(binaryDataBuffer);
            break;

        case krb::enums::KerberosMessageTypes::TGS_REP:
            printf("Classified as: TGS_REP\n");
            this->handleTGSRep(binaryDataBuffer);
            break;
        
        case krb::enums::KerberosMessageTypes::ERROR:
            printf("Classified as: Error\n");
            this->handleError(binaryDataBuffer);
            break;

        default: //Message type not recognized 
            printf("Message type not supported: %d\n", binaryDataBuffer[1]);
            return;
            break;
    }
}

void P2P::handleError(krb::NetworkByteArray &binaryDataBuffer)
{
    krb::KRB_ERROR error;
    error.deserialize(binaryDataBuffer);

    error.print();
}

void P2P::handleASRep(krb::NetworkByteArray &binaryDataBuffer)
{
    krb::AS_REP rep;
    rep.deserialize(binaryDataBuffer);

    rep.print();

    this->sendTGSRequest(rep);
}

void P2P::sendTGSRequest(krb::AS_REP &rep)
{
    rep.getTicket().print();
    

    krb::TGS_REQ requestTGS;
    requestTGS.setClientName(rep.getClientName());
    requestTGS.setServerName(krb::PrincipalName(krb::PrincipalNameTypes::NT_PRINCIPAL, krb::KerberosString(this->peer_name)));
    requestTGS.addTicket(rep.getTicket());

    if ((this->incoming_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error whilst creating socket to connect to AS.\n");
        return;
    }

    int check = inet_pton(AF_INET, TGS_IP, &this->address.sin_addr.s_addr);

    this->address.sin_family = AF_INET; //IPv4
    this->address.sin_port = htons(TGS_PORT);

    if(check <= 0) 
    {
        printf("Address not recognized or is invalid. Could not connect to Ticket-Granting-Server.\n");
        return;
    }
   
    //Connecting to the AS
    if (connect(this->incoming_socket, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
    {
        printf("Error when connecting to the socket\n");
        return;
    }

    //Connected to the AS
    printf("Connected\n");

    //Sending AS_REQ
    krb::NetworkByteArray obj;
    requestTGS.serialize(obj);

    requestTGS.print();
    
    printf("Sending..\n");

    unsigned char* data {nullptr};
    obj.getByteArray(data);

    send(this->incoming_socket, data, obj.getSize(), 0); //Sending the TGS_REQ

    free(data);

    this->listenForReply();
}

void P2P::handleTGSRep(krb::NetworkByteArray &binaryDataBuffer)
{
    printf("TODO: handleTGSRep\n");
}

void P2P::startSTUN()
{
    int stun_socket {};
    if ((stun_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) //IPv4, UDP
    {
        printf("Error whilst creating socket to connect to AS.\n");
        return;
    }

    int check = inet_pton(AF_INET, STUN_IP, &this->address.sin_addr.s_addr);

    this->address.sin_family = AF_INET; //IPv4
    this->address.sin_port = htons(STUN_PORT);

    if(check <= 0) 
    {
        printf("Address not recognized or is invalid. Could not connect to STUN-server.\n");
        return;
    }
   
    //Connecting to the STUN-server
    if (connect(stun_socket, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
    {
        printf("Error when connecting to the socket: %d\n", errno);
        return;
    }

    //Connected to the AS
    printf("Connected\n");

    

    //Sending STUN-Message
    stun::STUN_MESSAGE message;
    message.setMessageType(stun::StunMessageTypes::BINDING_REQUEST);
    krb::NetworkByteArray obj;
    message.serialize(obj);

    message.print();
    
    printf("Sending STUN-message...\n");

    unsigned char* data {nullptr};
    obj.getByteArray(data);

    send(stun_socket, data, obj.getSize(), 0); //Sending the AS_REQ

    free(data);

    memset(this->recive_buffer, 0, sizeof(this->recive_buffer)); //Nulling out the memory
    
    int bytesReceived {-2};
    std::thread t([&](){
        bytesReceived = recv(stun_socket, this->recive_buffer, sizeof(this->recive_buffer), 0);
    });
    
    //We wait for 10 seconds to get a response from the AS, if no response, the connection is considerd as failed
    bool timeout {false};
    auto time = std::chrono::system_clock::now();
    while (!t.joinable() && !timeout)
    {
        if (std::chrono::system_clock::now() - time > std::chrono::seconds(10) || bytesReceived != -2)
            timeout = true;
    }

    if (timeout)
    {
        printf("No response from the AS\n");
        this->shutdownConnection();
        t.join();
        return;
    }
    t.join();

    if (bytesReceived == -1)
    {
        printf("Error on recv() STUN: %d\n", errno);
        return;
    }
    if (bytesReceived == 0)
    {
        printf("Server end, TCP-connection is closed\n");
        return;
    }
    if (bytesReceived > 0)
    {
        /*printf("OBJ: ");
        for (int i = 0; i < bytesReceived; i++)
            printf("%x", this->recive_buffer[i]);
        printf("\n");*/
        krb::NetworkByteArray buffer = krb::NetworkByteArray((unsigned char*)this->recive_buffer, bytesReceived);
        stun::STUN_MESSAGE message;
        message.deserialize(buffer);
        message.print();

        stun::XOR_MAPPED_ADDRESS xor_address = stun::XOR_MAPPED_ADDRESS(message.attributes[0].value, message.attributes[0].length);
        xor_address.print();
    }
    printf("DONE\n");
}