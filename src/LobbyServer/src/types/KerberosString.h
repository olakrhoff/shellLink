#ifndef KERBEROS_STRING_H
#define KERBEROS_STRING_H

#include <string.h>


namespace krb
{
    class KerberosString
    {
        private: 
            krb::UInt32 length {};
            unsigned char *string {nullptr};

        public:
            KerberosString() {}

            KerberosString(std::string input_string)
            {
                this->string = (unsigned char*)malloc((size_t)input_string.length());

                for (std::string::iterator itr =  input_string.begin(); itr != input_string.end(); itr++)
                    this->string[length++] = (unsigned char)*itr;   
            }

            KerberosString(const KerberosString &other)
            {
                this->length = other.length;
                this->string = (unsigned char*)malloc(other.length);

                memcpy(this->string, other.string, (size_t)this->length);
            }

            KerberosString(KerberosString &&other)
            {
                this->length = other.length;
                this->string = other.string;
                other.string = nullptr;
            }

            ~KerberosString()
            {
                if (this->length > 0)
                    free(this->string);
            }

            KerberosString &operator=(const KerberosString &other)
            {
                this->length = other.length;
                free(this->string); //Free the old memory so it doesn't leak
                this->string = (unsigned char*)malloc(this->length);

                memcpy(this->string, other.string, (size_t)this->length);

                return *this;
            }

            KerberosString &operator=(KerberosString &&other)
            {
                this->length = other.length;
                free(this->string); //Free the old memory so it doesn't leak
                this->string = other.string;
                other.string = nullptr;

                return *this;
            }

            const char *getString()
            {
                return (const char *)this->string;
            }

            void serialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                binaryDataBuffer.addUInt32(this->length);
                binaryDataBuffer.addByteArray(this->string, this->length);
            }

            void deserialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                this->length = binaryDataBuffer.getNextUInt32();
                binaryDataBuffer.getNextNBytes(this->string, this->length);
            }

            void print()
            {
                printf("KerberosString:\nlength: %d\nstring: '", this->length);
                for (int i = 0; i < this->length; i++)
                    printf("%c", this->string[i]);
                printf("'\n");
            }
    };
}

#endif //KERBEROS_STRING_H