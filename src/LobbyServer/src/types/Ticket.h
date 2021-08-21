#ifndef TICKET_H
#define TICKET_H

#include "NetworkByteArray.h"
#include "global_consts.h"
#include "PrincipalName.h"
#include "EncryptedTicketPart.h"
#include "DeserializeException.h"

namespace krb
{
    class Ticket
    {
        private:
            krb::UInt8 tkt_vno = TKT_VNO; //Always 5
            krb::PrincipalName server_name;
            krb::EncryptedTicketPart enc_part;


        public:
            void serialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                binaryDataBuffer.addByte(this->tkt_vno);
                this->server_name.serialize(binaryDataBuffer);
                this->enc_part.serialize(binaryDataBuffer);
            }

            void deserialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                try
                {
                    this->tkt_vno = binaryDataBuffer.getNextByte();
                    this->server_name.deserialize(binaryDataBuffer);
                    this->enc_part.deserialize(binaryDataBuffer);
                }
                catch (...)
                {
                    throw DeserializeException();
                }
            }

            void print()
            {
                printf("Ticket:\ntkt_vno: %d\nserver_name:\n", this->tkt_vno);
                this->server_name.print();
                this->enc_part.print();
            }
    };
}

#endif //TICKET_H