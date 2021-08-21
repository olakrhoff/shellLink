#include "authenticationServer.h"

#include <arpa/inet.h>
#include <iostream>
#include <memory.h>
#include <thread>
#include <fstream>

#include "types/NetworkByteArray.h"
#include "types/DeserializeException.h"
#include "../../enviorment.h"
#include "types/as_rep.h"
#include "types/krb_error.h"
#include "types/ErrorCodes.h"
#include "types/KerberosMessageTypes.h"

AuthenticationServer::AuthenticationServer()
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
    
    while (true)
    {
        std::getline(file, line);
        if (line == "")
            return;
        this->users[this->number_of_users] = (char *)malloc(line.length());
        memcpy(this->users[this->number_of_users++], line.data(), line.length());
    }

    file.close();
    
}

AuthenticationServer::~AuthenticationServer()
{
    shutdown(this->socket_FD, SHUT_RDWR);
    shutdown(this->temp_socket, SHUT_RDWR);

    for (int i = 0; i < this->number_of_users; i++)
        free(this->users[i]);
}

void AuthenticationServer::start()
{
    this->createSocket();
    this->bindSocket();
    this->listenToSocket();
    while (this->acceptSocket())
        this->handleClient();
}

void AuthenticationServer::createSocket()
{
    this->socket_FD = socket(AF_INET, SOCK_STREAM, 0); //Ipv4, TCP

    if (this->socket_FD == -1) //Error occured while making the socket
    {
        printf("Failed to create socket\n");
        exit(1);
    }
}

void AuthenticationServer::bindSocket()
{
    if (bind(this->socket_FD, (struct sockaddr *)&this->address, sizeof(this->address)) == -1)
    {
        printf("AUTH\n");
        printf("Failed to bind the address to the socket (%d)\n", errno);
        this->~AuthenticationServer();
        exit(1);
    }
}

void AuthenticationServer::listenToSocket()
{
    if (listen(this->socket_FD, 10) == -1) //Setting the queue to 1
    {
        printf("Failed to listen to the socket\n");
        exit(1);
    }
}

bool AuthenticationServer::acceptSocket()
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

void AuthenticationServer::handleClient()
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

void AuthenticationServer::handleMessage(krb::NetworkByteArray &binaryDataBuffer)
{
    if (binaryDataBuffer[0] != PVNO) //Checking that the pvno (version number is correct)
    {
        printf("Message in unsupported version: %d", binaryDataBuffer[0]);
        return;
    }
    
    if (binaryDataBuffer[1] != krb::enums::KerberosMessageTypes::AS_REQ) //Message type is not AS_REQ
    {
        printf("Message type not supported: %d\n", binaryDataBuffer[1]);
        return;
    }
    printf("Classified as: AS_REQ\n");
    this->handleRequest(binaryDataBuffer);
     
}



void AuthenticationServer::handleRequest(krb::NetworkByteArray &binaryDataBuffer)
{
    //TODO: Validate that the format and values in the request are OK, this should be moved to the objects themself for simplicity
    //TODO: Authenticate the user, this is a bit furhter down the road when the cryptography is being implemented
    //TODO: Reply with a Ticket to the TGS
    krb::AS_REQ request;
    request.deserialize(binaryDataBuffer);

    request.print();    
    if (!this->doesUserExist(request.getClientName()))
    {
        krb::KRB_ERROR error;
        error.setErrorCode(krb::enums::ErrorCodes::KDC_ERR_C_PRINCIPAL_UNKNOWN);
        error.setServerName(request.getClientName());
        this->sendError(error);
        printf("User invalid\n");
        return;
    }
    printf("User found\n"); //This counts as authentication for now, the cryptography is not the main foucus yet, will be updated to accual authentication later

    if (!this->doesServerExist(request.getServerName()))
    {
        krb::KRB_ERROR error;
        error.setErrorCode(krb::enums::ErrorCodes::KDC_ERR_S_PRINCIPAL_UNKNOWN);
        error.setServerName(request.getServerName());
        this->sendError(error);
        printf("Server invalid\n");
        return;
    }
    printf("Server valid\n"); //Same as last "printf()"

    this->sendRep(request);
}

//Checks if the username sent by a user is in the list of users
bool AuthenticationServer::doesUserExist(krb::PrincipalName &&username)
{
    for (int i = 0; i < this->number_of_users; i++)
        if (strcmp(this->users[i], username.name_string.getString()) == 0)
            return true;
    return false;
}

//This method is a dummy method for now, it only simulates the correct behavior.
//It checks if the server name is "TGS" and only accepts this
bool AuthenticationServer::doesServerExist(krb::PrincipalName &&server_name)
{
    if (strcmp("TGS", server_name.name_string.getString()) == 0)
        return true;
    return false;
}

void AuthenticationServer::sendRep(krb::AS_REQ &request)
{
    krb::AS_REP rep;
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

void AuthenticationServer::sendError(krb::KRB_ERROR &error)
{
    error.print();
    

    krb::NetworkByteArray obj;
    error.serialize(obj);

    

    printf("Sending..\n");

    unsigned char* data {nullptr};
    obj.getByteArray(data);

    send(this->temp_socket, data, obj.getSize(), 0); //Sending the AS_REP

    free(data);
}