#ifndef KERBEROS_MESSAGE_TYPES_H
#define KERBEROS_MESSAGE_TYPES_H

//https://datatracker.ietf.org/doc/html/rfc4120#section-7.5.7
namespace krb
{
    namespace enums
    {
        enum KerberosMessageTypes
        {
            AS_REQ = 10,
            AS_REP = 11,
            TGS_REQ = 12,
            TGS_REP = 13,
            AP_REQ = 14,
            AP_REP = 15,
            RESERVED_16 = 16,
            RESERVED_17 = 17,
            SAFE = 20,
            PRIV = 21,
            CRED = 22,
            ERROR = 30
        };
    }
}
#endif //KERBEROS_MESSAGE_TYPES_H