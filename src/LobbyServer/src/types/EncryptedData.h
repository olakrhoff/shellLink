#ifndef ENCRYPTED_DATA_H
#define ENCRYPTED_DATA_H

#include "types.h"
#include "KerberosString.h"
#include "DeserializeException.h"

namespace krb
{
    class EncryptedData
    {
        protected:
            krb::Int32 encryption_type {};
            krb::UInt32 kvno {}; //Key version number   
            krb::KerberosString chiper;

        public:

            krb::KerberosString &getChiper()
            {
                return this->chiper;
            }

            void setChiper(krb::KerberosString &&chiper_value)
            {
                chiper = chiper_value;
            }

            virtual void serialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                binaryDataBuffer.addInt32(this->encryption_type);
                binaryDataBuffer.addUInt32(this->kvno);
                this->chiper.serialize(binaryDataBuffer);
            }

            virtual void deserialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                try
                {
                    this->encryption_type = binaryDataBuffer.getNextInt32();
                    this->kvno = binaryDataBuffer.getNextUInt32();
                    this->chiper.deserialize(binaryDataBuffer);
                }
                catch(...)
                {
                    throw DeserializeException();
                }
                
            }

            void print()
            {
                printf("EncryptedData:\nencryption_type: %d\nkvno: %d\n", this->encryption_type, this->kvno);
                this->chiper.print();
            }
    };
}

#endif //ENCRYPTED_DATA_H