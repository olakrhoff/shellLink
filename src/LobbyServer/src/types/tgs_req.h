#ifndef TGS_REQ_H
#define TGS_REQ_H

#include "kdc_req.h"
#include "global_consts.h"
#include "KerberosMessageTypes.h"

namespace krb
{
    //Kerberos ticket-granting-server request
    struct TGS_REQ
    {
        krb::KDC_REQ req;
    
        TGS_REQ()
        {
            this->req.msg_type = krb::enums::KerberosMessageTypes::TGS_REQ;
        }

        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            this->req.serialize(binaryDataBuffer);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            this->req.deserialize(binaryDataBuffer);   
        }

        void setKDCOptions(krb::UInt32 options)
        {
            this->req.req_body.kdc_options.setKDCOptions(options);
        }

        void setClientName(krb::PrincipalName &&client_name)
        {
            this->req.req_body.client_name = client_name;
        }

        void setClientName(krb::PrincipalName &client_name)
        {
            this->req.req_body.client_name = client_name;
        }

        void addTicket(krb::Ticket &ticket)
        {
            this->req.req_body.additonal_tickets.addTicket(ticket);
        }

        krb::PrincipalName getClientName()
        {
            return this->req.req_body.client_name;
        }

        void setServerName(krb::PrincipalName &&server_name)
        {
            this->req.req_body.server_name = server_name;
        }

        krb::PrincipalName getServerName()
        {
            return this->req.req_body.server_name;
        }

        //Sets how many hours a request is valid for
        void setTill()
        {

        }

        void setNonce(krb::UInt32 nonce_value)
        {
            this->req.req_body.nonce = nonce_value;
        }

        void setEncryptedData()
        {

        }

        void setAdditionalTickets()
        {

        }

        void print()
        {
            printf("TGS_REQ:\n");
            this->req.print();
        }
    };
}

#endif //TGS_REQ_H