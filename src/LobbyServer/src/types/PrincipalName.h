#ifndef PRINCIPAL_NAME_H
#define PRINCIPAL_NAME_H

#include "types.h"
#include "global_consts.h"
#include "KerberosString.h"

namespace krb
{
    enum PrincipalNameTypes
    {
        NT_UNKNOWN = 0,
        NT_PRINCIPAL = 1,
        NT_SRV_INST = 2,
        NT_SRV_HST = 3,
        NT_SRV_XHST = 4,
        NT_UID = 5,
        NT_X500_PRINCIPAL = 6,
        NT_SMTP_NAME = 7,
        NT_ENTERPRISE = 10
    };

    struct PrincipalName
    {
        krb::Int32 name_type {};
        krb::KerberosString name_string {};

        PrincipalName() {};

        PrincipalName(krb::PrincipalNameTypes type, krb::KerberosString &&stringData)
        {
            this->name_type = type;
            this->name_string = stringData;
        }
        
        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            binaryDataBuffer.addInt32(this->name_type);
            this->name_string.serialize(binaryDataBuffer);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            this->name_type = binaryDataBuffer.getNextInt32();
            this->name_string.deserialize(binaryDataBuffer);
        }

        void print()
        {
            printf("PrincipalName:\nname_type: %d\n", this->name_type);
            this->name_string.print();
        }
    };
}

#endif //PRINCIPAL_NAME_H