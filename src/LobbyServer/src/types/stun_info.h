#ifndef STUN_INFO_H
#define STUN_INFO_H

#include "types.h"
#include "KerberosString.h"
#include "NetworkByteArray.h"
#include "IllegalArgumentException.h"

namespace krb
{
    struct STUN_INFO
    {
        krb::UInt32 ipv4 {}; //IPv4
        krb::UInt16 port {}; //Port

        krb::KerberosString getAsKerberosString()
        {
            krb::NetworkByteArray buffer;
            buffer.addByte(6);
            buffer.addUInt32(this->ipv4);
            buffer.addUInt16(this->port);

            krb::KerberosString string;
            string.deserialize(buffer);

            return string;
        }

        void setFromKerberosString(krb::KerberosString &string)
        {
            printf("\nKAKE\n");
            krb::NetworkByteArray buffer;
            string.deserialize(buffer);
            
            int length = buffer.getNextByte(); //The length of the string
            if (length != 6)
                throw IllegalArgumentException("STUN_INFO::setFromKerberosString()");
            
            this->ipv4 = buffer.getNextUInt32();
            this->port = buffer.getNextUInt16();
        }
    };
}

#endif //STUN_INFO_H