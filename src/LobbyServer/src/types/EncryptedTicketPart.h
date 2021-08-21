#ifndef ENCRYPTED_TICKET_PART_H
#define ENCRYPTED_TICKET_PART_H

#include "types.h"
#include "EncryptionKey.h"
#include "PrincipalName.h"
#include "KerberosTime.h"
#include "EncryptedData.h"
#include "NetworkByteArray.h"

namespace krb
{
    class EncryptedTicketPart : public EncryptedData
    {
        private:
            krb::UInt32 flags; //TODO: implement this correctly (as TicketFlags (KerberosFlags))
            krb::EncryptionKey key;
            krb::PrincipalName client_name;
            krb::KerberosTime auth_time;
            krb::KerberosTime end_time;

        public:
            void serialize(krb::NetworkByteArray &binaryDataBuffer) override
            {
                EncryptedData::chiper = krb::KerberosString("TODO: Actual encryption");
                EncryptedData::serialize(binaryDataBuffer);
            }

            void deserialize(krb::NetworkByteArray &binaryDataBuffer) override
            {
                EncryptedData::deserialize(binaryDataBuffer);
            }
            
    };
}

#endif //ENCRYPTED_TICKET_PART_H