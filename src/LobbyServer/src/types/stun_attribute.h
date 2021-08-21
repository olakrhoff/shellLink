#ifndef STUN_ATTRIBUTE_H
#define STUN_ATTRIBUTE_H

#include "types.h"
#include "stun_attribute_types.h"
#include "IllegalArgumentException.h"

namespace stun
{
    struct STUN_ATTRIBUTE
    {
        public:
            krb::UInt16 type {};
            krb::UInt16 length {};
            unsigned char *value {nullptr}; //Always a multiple of 32-bit

            void serialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                binaryDataBuffer.addUInt16(this->type);
                binaryDataBuffer.addUInt16(this->length);
                binaryDataBuffer.addByteArray(this->value, this->length);
            }

            void deserialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                this->type = binaryDataBuffer.getNextUInt16();
                if(!this->checkType())
                    throw krb::IllegalArgumentException("STUN_ATTRIBUTE::deserialize(NetworkByteArray): Attribute type not supported");
                this->length = binaryDataBuffer.getNextUInt16();
                binaryDataBuffer.getNextNBytes(this->value, this->length);
            }

            void print()
            {
                printf("STUN_ATTRIBUTE:\ntype: %d\nlength: %d\n", this->type, this->length);
            }
        
        private:
            bool checkType()
            {
                switch (this->type)
                {
                    case stun::enums::StunAttributeTypes::XOR_MAPPED_ADDRESS:
                        return true;
                        break;
                    //TODO: Add handliing of other attribute types
                    default:
                        return false;
                        break;
                }
            }
    };
}

#endif //STUN_ATTRIBUTE_H