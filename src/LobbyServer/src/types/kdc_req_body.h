#ifndef KRB_KDC_REQ_BODY_H
#define KRB_KDC_REQ_BODY_H

#pragma pack(push, 2)

#include <vector>
#include <time.h>
#include <stdlib.h>

#include "global_consts.h"
#include "types.h"
#include "krb_kdc_options.h"
#include "KerberosTime.h"
#include "PrincipalName.h"
#include "EncryptedData.h"
#include "Tickets.h"
#include "NetworkByteArray.h"
#include "DeserializeException.h"

namespace krb
{
    //The body of the KDC_REQ
    struct KDC_REQ_BODY
    {
        KRB_KDC_OPTIONS kdc_options;
        krb::PrincipalName client_name; //only used in the AS_REQ
        krb::PrincipalName server_name;
        krb::KerberosTime till;
        krb::UInt32 nonce {(krb::UInt32)(rand() * std::time(nullptr))}; //Random number
        krb::EncryptedData enc_authoriaztion_data;
        krb::Tickets additonal_tickets;


        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            binaryDataBuffer.addUInt32(this->kdc_options.getKDCOptions());
            this->client_name.serialize(binaryDataBuffer);
            this->server_name.serialize(binaryDataBuffer);
            this->till.serialize(binaryDataBuffer);
            binaryDataBuffer.addUInt32(this->nonce);
            this->enc_authoriaztion_data.serialize(binaryDataBuffer);
            this->additonal_tickets.serialize(binaryDataBuffer);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            this->kdc_options.setKDCOptions(binaryDataBuffer.getNextUInt32());
            this->client_name.deserialize(binaryDataBuffer);
            this->server_name.deserialize(binaryDataBuffer);
            this->till.deserialize(binaryDataBuffer);
            this->nonce = binaryDataBuffer.getNextUInt32();
            this->enc_authoriaztion_data.deserialize(binaryDataBuffer);
            this->additonal_tickets.deserialize(binaryDataBuffer);
        }

        void print()
        {
            printf("KDC_REQ_BODY:\n");
            this->kdc_options.print();
            printf("client_name: ");
            this->client_name.print();
            printf("\nserver_name: ");
            this->server_name.print();
            printf("\ntill: ");
            this->till.print();
            printf("\nnonce: %d\n", this->nonce);
            printf("enc_authoriaztion_data: ");
            this->enc_authoriaztion_data.print();
            printf("\nadditonal_tickets: ");
            this->additonal_tickets.print();
            printf("\n");
        }
    };
}

#pragma pack(pop)

#endif //KRB_KDC_REQ_BODY_H