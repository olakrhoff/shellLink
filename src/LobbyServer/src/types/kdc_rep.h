#ifndef KDC_REP_H
#define KDC_REP_H

#include <vector>

#include "types.h"
#include "global_consts.h"
#include "PrincipalName.h"
#include "Ticket.h"
#include "EncryptedData.h"
#include "NetworkByteArray.h"
#include "DeserializeException.h"

namespace krb
{
    struct KDC_REP
    {
        krb::UInt8 pvno = PVNO;
        krb::UInt8 msg_type; //AS: 11, TGS: 13
        krb::PrincipalName client_name; //When AS responds to Client this is the name of the Client
        krb::Ticket ticket; //When AS repsonds to Client, it is the Ticket-Granting-Ticket (TGT), this ticket is encrypted with the long term key between the AS and the TGS.
        krb::EncryptedData encrypted_part;

        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            binaryDataBuffer.addByte(this->pvno);
            binaryDataBuffer.addByte(this->msg_type);
            this->client_name.serialize(binaryDataBuffer);
            this->ticket.serialize(binaryDataBuffer);
            this->encrypted_part.serialize(binaryDataBuffer);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            try
            {
                this->pvno = binaryDataBuffer.getNextByte();
                this->msg_type = binaryDataBuffer.getNextByte();
                this->client_name.deserialize(binaryDataBuffer);
                this->ticket.deserialize(binaryDataBuffer);
                this->encrypted_part.deserialize(binaryDataBuffer);
            }
            catch (...)
            {
                throw DeserializeException();
            }
        }

        void print()
        {
            printf("KDC_REP:\npvno: %d\nmsg_type: %d\n", this->pvno, this->msg_type);
            this->client_name.print();
            this->ticket.print();
            this->encrypted_part.print();
        }
    };
}

#endif //KDC_REP_H