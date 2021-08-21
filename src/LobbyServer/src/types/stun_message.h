#ifndef STUN_MESSAGE_H
#define STUN_MESSAGE_H

#include "types.h"
#include "stun_message_types.h"
#include "stun_transaction_id.h"
#include "stun_attribute.h"
#include "NetworkByteArray.h"
#include "IllegalArgumentException.h"

#define MAGIC_COOKIE 0x2112A442 //MUST be, say by protocol

namespace stun
{
    struct STUN_MESSAGE
    {
        public:
            krb::UInt16 msg_type {}, msg_length {};
            krb::UInt32 magic_cookie {MAGIC_COOKIE};
            stun::TransactionID transaction_id;
            int number_of_attributes {};
            stun::STUN_ATTRIBUTE *attributes;

            void setMessageType(stun::StunMessageTypes type)
            {
                this->msg_type = type;
            }

            void setMessageLength(krb::UInt16 length)
            {
                this->msg_length = length;
            }

            void serialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                binaryDataBuffer.addUInt16(this->msg_type);
                binaryDataBuffer.addUInt16(this->msg_length);
                binaryDataBuffer.addUInt32(this->magic_cookie);
                this->transaction_id.serialize(binaryDataBuffer);
            }

            void deserialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                this->msg_type = binaryDataBuffer.getNextUInt16();
                if (!this->checkMessageType())
                    throw krb::IllegalArgumentException("STUN_MESSAGE::deserialize(NetworkByteArray): Message-type format wrong");
                this->msg_length = binaryDataBuffer.getNextUInt16();
                if (this->msg_length % 4 != 0)
                    throw krb::IllegalArgumentException("STUN_MESSAGE::deserialize(NetworkByteArray): mag_length is not a multiple of 4 byte, format error");
                this->magic_cookie = binaryDataBuffer.getNextUInt32();
                if (this->magic_cookie != MAGIC_COOKIE)
                    throw krb::IllegalArgumentException("STUN_MESSAGE::deserialize(NetworkByteArray): Magic cookie value is not correct");
                this->transaction_id.deserialize(binaryDataBuffer);

                if (this->msg_length > 0)
                    this->getAttributes(binaryDataBuffer);
            }

            void print()
            {
                printf("STUN_MESSAGE:\nmsg_type: %d\nmsg_length: %d\nmagic_cookie: %d\n", this->msg_type, this->msg_length, this->magic_cookie);
                this->transaction_id.print();
                printf("attributes:\n");
                for (int i = 0; i < this->number_of_attributes; i++)
                    this->attributes[i].print();
            }

        private:
            bool checkMessageType()
            {
                switch (this->msg_type)
                {
                    case stun::StunMessageTypes::BINDING_SUCCESS_RESPONSE:
                        printf("BINDING_SUCCESS_RESPONSE\n");
                        return true;
                        break;
                    
                    default:
                        return false;
                        break;
                }
            }

            void getAttributes(krb::NetworkByteArray &binaryDataBuffer)
            {
                int left = this->msg_length;
                stun::STUN_ATTRIBUTE attribute;
                attribute.deserialize(binaryDataBuffer);
                this->expand();
                this->attributes[this->number_of_attributes++] = attribute;
            }

            //Adds room for one more attribute, not optimal,but temporary solution
            void expand()
            {
                if (this->attributes == nullptr)
                {
                    this->attributes = (stun::STUN_ATTRIBUTE *)malloc(sizeof(stun::STUN_ATTRIBUTE));
                    return;
                }

                stun::STUN_ATTRIBUTE *temp = this->attributes;
                printf("TEST\n");
                free(attributes);
                printf("TEST\n");
                this->attributes = (stun::STUN_ATTRIBUTE *)malloc(sizeof(stun::STUN_ATTRIBUTE) * this->number_of_attributes);
                
                memcpy(this->attributes, temp, this->number_of_attributes);
            }
    };
}

#endif //STUN_MESSAGE_H