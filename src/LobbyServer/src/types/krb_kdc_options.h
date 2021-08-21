#ifndef KRB_KDC_OPTIONS_H
#define KRB_KDC_OPTIONS_H

#include "types.h"

namespace krb
{
    struct KRB_KDC_OPTIONS
    {
        bool reserved {0}; //0
        bool forwardable {0}; //1
        bool forwarded {0}; //2
        bool proxiable {0}; //3
        bool proxy {0}; //4
        bool allow_postdate {0}; //5
        bool postdated {0}; //6
        //bool unused7; //7
        bool renewable {0}; //8
        //bool unused9; //9
        //bool unused10; //10
        bool opt_hardware_auth {0}; //11
        //bool unused12; //12
        //bool unused13; //13
        //bool unused15; //15 is reserved for canonicalize
        bool disable_transited_check {0}; //26
        bool renewable_ok {0}; //27
        bool enc_tkt_in_skey {0}; //28
        bool renew {0}; //30
        bool validate {0}; //31

        void setKDCOptions(krb::UInt32 value = 0)
        {
            this->reserved = value & (1);
            this->forwardable = value & (1 << 1);
            this->forwarded = value & (1 << 2);
            this->proxiable = value & (1 << 3);
            this->proxy = value & (1 << 4);
            this->allow_postdate = value & (1 << 5);
            this->postdated = value & (1 << 6);
            this->renewable = value & (1 << 8);
            this->opt_hardware_auth = value & (1 << 11);
            this->disable_transited_check = value & (1 << 26);
            this->renewable_ok = value & (1 << 27);
            this->enc_tkt_in_skey = value & (1 << 28);
            this->renew = value & (1 << 30);
            this->validate = value & (1 << 31);
        }


        //This method turns A KRB_KDC_OPTIONS struct into a more compact 32 bit integer
        //The struct is easier to work with, but not optimal to send as an object over the network
        krb::UInt32 getKDCOptions()
        {
            krb::UInt32 flags {};
            flags |= this->reserved;
            flags |= ((int)this->forwardable << 1);
            flags |= ((int)this->forwarded << 2);
            flags |= ((int)this->proxiable << 3);
            flags |= ((int)this->proxy << 4);
            flags |= ((int)this->allow_postdate << 5);
            flags |= ((int)this->postdated << 6);
            flags |= ((int)this->renewable << 8);
            flags |= ((int)this->opt_hardware_auth << 11);
            flags |= ((int)this->disable_transited_check << 26);
            flags |= ((int)this->renewable_ok << 27);
            flags |= ((int)this->enc_tkt_in_skey << 28);
            flags |= ((int)this->renew << 30);
            flags |= ((int)this->validate << 31);
            
            return flags;
        }

        void print()
        {
            printf("KRB_KDC_OPTIONS: %d\n", this->getKDCOptions());
        }
    };
}

#endif //KRB_KDC_OPTIONS_H