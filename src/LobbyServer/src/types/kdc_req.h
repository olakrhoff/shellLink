#ifndef KRB_KDC_REQ_H
#define KRB_KDC_REQ_H

#pragma pack(push, 2)

#include <vector>

#include "global_consts.h"
#include "types.h"
#include "kdc_req_body.h"
#include "NetworkByteArray.h"
#include "DeserializeException.h"

namespace krb
{
    //Kerberos key distribution center request, super type for all requests to the KDC, hens to the AS and TGS.
    struct KDC_REQ
    {
        krb::UInt8 pvno = PVNO; //Always 5
        krb::UInt8 msg_type; //AS: 10, TGS: 12
        krb::KDC_REQ_BODY req_body;


        void serialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            binaryDataBuffer.addByte(this->pvno);
            binaryDataBuffer.addByte(this->msg_type);
            this->req_body.serialize(binaryDataBuffer);
        }

        void deserialize(krb::NetworkByteArray &binaryDataBuffer)
        {
            try
            {
                this->pvno = binaryDataBuffer.getNextByte();
                this->msg_type = binaryDataBuffer.getNextByte();
                this->req_body.deserialize(binaryDataBuffer);
            }
            catch (...)
            {
                throw DeserializeException();
            }
        }

        void print()
        {
            printf("KDC_REQ:\npvno: %d\nmsg_type: %d\n", this->pvno, this->msg_type);
            this->req_body.print();
        }
    };
}
#pragma pack(pop)

#endif //KRB_KDC_REQ_H