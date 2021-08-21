#ifndef KRB_ERROR_H
#define KRB_ERROR_H

#include "types.h"
#include "global_consts.h"
#include "KerberosTime.h"
#include "KerberosString.h"
#include "ErrorCodes.h"
#include "KerberosMessageTypes.h"
#include "NetworkByteArray.h"
#include "DeserializeException.h"
#include "PrincipalName.h"

namespace krb
{
    struct KRB_ERROR
    {
        krb::UInt8 pvno = PVNO;
        krb::UInt8 msg_type {krb::enums::KerberosMessageTypes::ERROR};
        krb::KerberosTime server_time;
        krb::Int32 error_code {krb::enums::ErrorCodes::KDC_ERR_NONE};
        krb::PrincipalName server_name;
        krb::KerberosString e_text; //A Text to explain the problem (Optional)

        void setErrorCode(krb::enums::ErrorCodes code)
        {
            this->error_code = code;
            this->setETextBasedOnCode();
        }

        void setServerName(krb::PrincipalName &&server_name)
        {
            this->server_name = server_name;
        }

        void setEText(krb::KerberosString text)
        {
            //TODO: Impement this
        }

        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            binaryDataBuffer.addByte(this->pvno);
            binaryDataBuffer.addByte(this->msg_type);
            this->server_time.serialize(binaryDataBuffer);
            binaryDataBuffer.addInt32(this->error_code);
            this->server_name.serialize(binaryDataBuffer);
            this->e_text.serialize(binaryDataBuffer);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            try
            {
                this->pvno = binaryDataBuffer.getNextByte();
                this->msg_type = binaryDataBuffer.getNextByte();
                this->server_time.deserialize(binaryDataBuffer);
                this->error_code = binaryDataBuffer.getNextInt32();
                this->server_name.deserialize(binaryDataBuffer);
                this->e_text.deserialize(binaryDataBuffer);
            }
            catch (...)
            {
                throw DeserializeException();
            }   
        }

        void print()
        {
            printf("KRB_ERROR:\npvno: %d\nmsg_type: %d\n", this->pvno, this->msg_type);
            this->server_time.print();
            printf("error_code: %d\n", this->error_code);
            this->server_name.print();
            this->e_text.print();
        }

        private:
            void setETextBasedOnCode()
            {
                switch (this->error_code)
                {
                    case krb::enums::ErrorCodes::KDC_ERR_NONE:
                        this->e_text = krb::KerberosString("No error");
                        break;
                    
                    case krb::enums::ErrorCodes::KDC_ERR_C_PRINCIPAL_UNKNOWN:
                        this->e_text = krb::KerberosString("Client not found in Kerberos database");
                        break;

                    case krb::enums::ErrorCodes::KDC_ERR_S_PRINCIPAL_UNKNOWN:
                        this->e_text = krb::KerberosString("Server not found in Kerberos database");
                        break;

                    default:
                        break;
                }
            }
    };
}

#endif //KRB_ERROR_H