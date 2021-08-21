#ifndef XOR_MAPPED_ADDRESS_H
#define XOR_MAPPED_ADDRESS_H

#include "../types.h"
#include "../NetworkByteArray.h"
#include "../IllegalArgumentException.h"

namespace stun
{
    struct XOR_MAPPED_ADDRESS
    {
        krb::UInt8 reserved {0}, family {};
        krb::UInt16 port {};
        krb::UInt32 address[4] {}; //If IPv4 just read the first, if IPv6 read all

        XOR_MAPPED_ADDRESS(unsigned char *value, int length)
        {
            krb::NetworkByteArray data;
            data.addByteArray(value, length);
            
            this->reserved = data.getNextByte();
            if (this->reserved != 0)
                throw krb::IllegalArgumentException("XOR_MAPPED_ADDRESS::XOR_MAPPED_ADDRESS(char *, int): reserved is not valid (0)");
            this->family = data.getNextByte();
            if (this->family != 0x01 && this->family != 0x02)
                throw krb::IllegalArgumentException("XOR_MAPPED_ADDRESS::XOR_MAPPED_ADDRESS(char *, int): famliy type is not valid (1 or 2)");
            this->port = data.getNextUInt16();
            if (this->family == 0x01) //IPv4
                this->address[0] = data.getNextUInt32();
            if (this->family == 0x02) //IPv6
                for (int i = 0; i < 3; i++)
                    this->address[i] = data.getNextUInt32();
        }

        void print()
        {
            printf("XOR_MAPPED_ADDRESS:\nreserved: %d\nfamily: %d\nport: %d\naddress: ", this->reserved, this->family, this->port);
            if (this->family == 0x01)
                printf("%d\n", this->address[0]);
            else
                printf("IPv6\n");
        }
    };
}

#endif //XOR_MAPPED_ADDRESS_H