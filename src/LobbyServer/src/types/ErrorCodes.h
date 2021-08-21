#ifndef ERROR_CODES_H
#define ERROR_CODES_H

//https://datatracker.ietf.org/doc/html/rfc4120#section-7.5.9

namespace krb
{
    namespace enums
    {
        enum ErrorCodes
        {
            KDC_ERR_NONE = 0,
            KDC_ERR_C_PRINCIPAL_UNKNOWN = 6,
            KDC_ERR_S_PRINCIPAL_UNKNOWN = 7
        };
    }
}


#endif //ERROR_CODES_H