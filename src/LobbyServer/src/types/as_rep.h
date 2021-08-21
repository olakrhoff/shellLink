#ifndef AS_REP_H
#define AS_REP_H

#include "kdc_rep.h"
#include "global_consts.h"
#include "NetworkByteArray.h"
#include "KerberosMessageTypes.h"
#include "DeserializeException.h"
#include "PrincipalName.h"

namespace krb
{
    //Kerberos authentication-server recipt
    struct AS_REP
    {
        krb::KDC_REP rep;

        AS_REP()
        {
            this->rep.msg_type = krb::enums::KerberosMessageTypes::AS_REP;
        }

        void setTicket()
        {

        }

        krb::PrincipalName getClientName()
        {
            return this->rep.client_name;
        }
        
        void setClientName(krb::PrincipalName &&client_name)
        {
            this->rep.client_name = client_name;
        }

        krb::Ticket &getTicket()
        {
            return this->rep.ticket;
        }

        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            this->rep.serialize(binaryDataBuffer);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            try
            {
                this->rep.deserialize(binaryDataBuffer);
            }
            catch (...)
            {
                throw DeserializeException();
            }   
        }

        void print()
        {
            printf("AS_REP:\n");
            this->rep.print();
        }
    };
}

#endif //AS_REP_H