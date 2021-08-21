#ifndef STUN_ATTRIBUTE_TYPES_H
#define STUN_ATTRIBUTE_TYPES_H

namespace stun
{
    namespace enums
    {
        enum StunAttributeTypes
        {
            MAPPED_ADDRESS = 0x0001,
            USERNAME = 0x0006,
            MESSAGE_INTEGRITY = 0x0008,
            ERROR_CODE = 0x0009,
            UNKNOWN_ATTRIBUTE = 0x000A,
            REALM = 0x0014,
            NONCE = 0x0015,
            XOR_MAPPED_ADDRESS = 0x0020,
            SOFTWARE = 0x8022,
            ALTERATE_SERVER = 0x8023,
            FINGERPRINT = 0x8028
        };
    }
}

#endif //STUN_ATTRIBUTE_TYPES_H