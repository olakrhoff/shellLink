#include "ticketGrantingServer.h"

#include <fstream>
#include <iostream>
#include <memory.h>
#include <arpa/inet.h>
#include <thread>

#include "../../enviorment.h"
#include "types/NetworkByteArray.h"
#include "types/tgs_rep.h"
#include "types/IllegalArgumentException.h"

TicketGrantingServer::TicketGrantingServer()
{
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(this->port); //Networking is BIG_ENDIAN and the machine is LITTLE_ENDIAN, hotns: HostTONetworkShort
    int check = inet_pton(AF_INET, this->ip, &this->address.sin_addr.s_addr);

    if(check <= 0) 
    {
        printf("Address not recognized or is invalid\n");
        exit(1);
    }

    std::string fileName = ENV_PATH, line;
    fileName.append("/src/LobbyServer/src/users.txt");

    std::fstream file;

    file.open(fileName, std::ios::in);

    if (!file.is_open())
    {
        printf("Error fetching users from file\n");
        exit(1);
    }   

    //NOTE: If this were to be a scaleable solution it would use a database to store all the usernames and send request to
    //the database to se if the user existed. However there isn't going to be more than a few test users so this quicker
    //implemnetation will work fine (reading from file).
    this->users = (char **)malloc(this->users_size);
    for (int i = 0; i < this->users_size; i++)
        this->users[i] = nullptr;

    while (true)
    {
        std::getline(file, line);
        if (line == "")
            break;
        if (this->number_of_users > this->users_size)
        {
            char **temp = this->users;
            this->users_size = this->users_size << 1;
            this->users = (char **)malloc(this->users_size);
            
            for (int i = 0; i < this->users_size; i++)
                this->users[i] = nullptr;

            for (int i = 0; i < this->number_of_users; i++)
            {
                this->users[i] = (char *)malloc(sizeof(temp[i]));
                strcpy(this->users[i], temp[i]);
                free(temp[i]);
            }
            free(temp);
        }
        this->users[this->number_of_users] = (char *)malloc(line.length());
        memcpy(this->users[this->number_of_users++], line.data(), line.length());
    }

    file.close();

    krb::STUN_INFO info[this->number_of_users];
    this->user_stun_info = info;
}

TicketGrantingServer::~TicketGrantingServer()
{
    shutdown(this->socket_FD, SHUT_RDWR);
    shutdown(this->temp_socket, SHUT_RDWR);

    for (int i = 0; i < this->number_of_users; i++)
        free(this->users[i]);
    free(this->users);
}

void TicketGrantingServer::start()
{
    this->createSocket();
    this->bindSocket();
    this->listenToSocket();
    while (this->acceptSocket())
        this->handleClient();
}

void TicketGrantingServer::createSocket()
{
    this->socket_FD = socket(AF_INET, SOCK_STREAM, 0); //Ipv4, TCP

    if (this->socket_FD == -1) //Error occured while making the socket
    {
        printf("Failed to create socket\n");
        exit(1);
    }
}

void TicketGrantingServer::bindSocket()
{
    if (bind(this->socket_FD, (struct sockaddr *)&this->address, sizeof(this->address)) == -1)
    {
        printf("TICKET\n");
        printf("Failed to bind the address to the socket (%d)\n", errno);
        this->~TicketGrantingServer();
        exit(1);
    }
}

void TicketGrantingServer::listenToSocket()
{
    if (listen(this->socket_FD, 10) == -1) //Setting the queue to 1
    {
        printf("Failed to listen to the socket\n");
        exit(1);
    }
}

bool TicketGrantingServer::acceptSocket()
{
    printf("Connecting...\n");
    if ((this->temp_socket = accept(this->socket_FD, (struct sockaddr *)&this->address, (socklen_t*)&this->addrlen)) == -1)
    {
        printf("Failed to accept socket connection\n");
        return false;
    }
    printf("Connected\n");
    return true;
}

void TicketGrantingServer::handleClient()
{
    printf("Handling client\n");
    memset(this->recive_buffer, 0, sizeof(this->recive_buffer)); //Nulling out the memory
    memset(this->send_buffer, 0, sizeof(this->send_buffer)); //Nulling out the memory

    while (true)
    {
        memset(this->recive_buffer, 0, sizeof(this->recive_buffer)); //Nulling out the memory
        int bytesReceived = recv(this->temp_socket, this->recive_buffer, sizeof(this->recive_buffer), 0);

        if (bytesReceived == -1)
        {
            printf("Error on recv()\n");
            return;
        }
        if (bytesReceived == 0)
        {
            printf("Client end, TCP-connection is closed\n");
            return;
        }

        printf("Client (%d):\n", bytesReceived);
        
        printf("OBJ: '");
        for (int i = 0; i < bytesReceived; i++)
            printf("%x, ", (unsigned char)this->recive_buffer[i]);
        printf("'\n");

        krb::NetworkByteArray network_byte_array = krb::NetworkByteArray((unsigned char*)this->recive_buffer, bytesReceived);
        this->handleMessage(network_byte_array);
    }
}

void TicketGrantingServer::handleMessage(krb::NetworkByteArray &binaryDataBuffer)
{
    if (binaryDataBuffer[0] != PVNO) //Checking that the pvno (version number is correct)
    {
        printf("Message in unsupported version: %d\n", binaryDataBuffer[0]);
        return;
    }
    
    if (binaryDataBuffer[1] != krb::enums::KerberosMessageTypes::TGS_REQ) //Message type is not TGS_REQ
    {
        printf("Message type not supported: %d\n", binaryDataBuffer[1]);
        return;
    }
    printf("Classified as: TGS_REQ\n");
    this->handleRequest(binaryDataBuffer);
     
}



void TicketGrantingServer::handleRequest(krb::NetworkByteArray &binaryDataBuffer)
{
    //TODO: Validate that the format and values in the request are OK, this should be moved to the objects themself for simplicity
    //TODO: Authenticate the user, this is a bit further down the road when the cryptography is being implemented
    //TODO: Reply with the info the user needs
    krb::TGS_REQ request;
    request.deserialize(binaryDataBuffer);

    if (DEBUG)
        request.print();    
    int found_user = this->doesUserExist(request.getClientName()), found_peer;
    if (found_user == -1)
    {
        krb::KRB_ERROR error;
        error.setErrorCode(krb::enums::ErrorCodes::KDC_ERR_C_PRINCIPAL_UNKNOWN);
        error.setServerName(request.getClientName());
        this->sendError(error);
        printf("User invalid\n");
        return;
    }
    printf("User found\n"); //This counts as authentication for now, the cryptography is not the main foucus yet, will be updated to accual authentication later

    found_peer = this->doesUserExist(request.getServerName());
    if (found_peer == -1) //Server_name in a TGS_REQ is the name of the peer user
    {
        krb::KRB_ERROR error;
        error.setErrorCode(krb::enums::ErrorCodes::KDC_ERR_S_PRINCIPAL_UNKNOWN);
        error.setServerName(request.getServerName());
        this->sendError(error);
        printf("Peer invalid\n");
        return;
    }
    printf("Peer valid\n"); //Same as last "printf()"

    //Ask for stun info
    //Update stun info
    //Check connection table, if a match, share stun info with peers to enable peer-to-peer connection


    this->sendRep(request);
}

//Checks if the username sent by a user is in the list of users
int TicketGrantingServer::doesUserExist(krb::PrincipalName &&username)
{
    for (int i = 0; i < this->number_of_users; i++)
        if (strcmp(this->users[i], username.name_string.getString()) == 0)
            return i;
    return -1;
}

void TicketGrantingServer::sendRep(krb::TGS_REQ &request)
{
    krb::TGS_REP rep;
    rep.setClientName(request.getClientName());

    rep.print();
    

    krb::NetworkByteArray obj;
    rep.serialize(obj);

    

    printf("Sending..\n");

    unsigned char* data {nullptr};
    obj.getByteArray(data);

    send(this->temp_socket, data, obj.getSize(), 0); //Sending the AS_REP

    free(data);
}

void TicketGrantingServer::sendError(krb::KRB_ERROR &error)
{
    error.print();
    

    krb::NetworkByteArray obj;
    error.serialize(obj);

    

    printf("Sending...\n");

    unsigned char* data {nullptr};
    obj.getByteArray(data);

    send(this->temp_socket, data, obj.getSize(), 0); //Sending the AS_REP

    free(data);
}

//Checks if there is STUN-info about the suer stored, if it is a pointer
//to the STUN_INFO will be returned, otherwise a nullpointer will be returned
krb::STUN_INFO *TicketGrantingServer::checkUserSTUN(int user_index)
{
    if (user_index >= this->users_size)
        throw krb::IllegalArgumentException("TicketGrantingServer::checkUserSTUN(int), index out of bound");

    return &this->user_stun_info[user_index];    
}