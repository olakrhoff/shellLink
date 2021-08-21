#ifndef STUN_TRANSACTION_ID_H
#define STUN_TRANSACTION_ID_H

#include "types.h"
#include "NetworkByteArray.h"

namespace stun
{
    //A 96 bit id used in a STUN-message
    struct TransactionID
    {
        krb::UInt32 first {}, second {}, third {};

        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            binaryDataBuffer.addUInt32(this->first);
            binaryDataBuffer.addUInt32(this->second);
            binaryDataBuffer.addUInt32(this->third);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            this->first = binaryDataBuffer.getNextUInt32();
            this->second = binaryDataBuffer.getNextUInt32();
            this->third = binaryDataBuffer.getNextUInt32();
        }

        void print()
        {
            printf("TransactionID: %d : %d : %d\n", this->first, this->second, this->third);
        }   
    };
}

#endif //STUN_TRANSACTION_ID_H