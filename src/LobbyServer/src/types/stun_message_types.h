#ifndef STUN_MESSAGE_TYPES_H
#define STUN_MESSAGE_TYPES_H

namespace stun
{
    enum StunMessageTypesClasses
    {
        REQUEST = 0, //0b0000000000000000
        INDICATION = 16, //0b0000000000010000
        SUCCESS_RESPONSE = 256, //0b0000000100000000
        ERROR_RESPONSE = 272 //0b0000000100010000
    };

    enum StunMessageTypesMethods
    {
        BINDING = 1, //0b0000000000000001
    };

    enum StunMessageTypes
    {
        BINDING_REQUEST = stun::StunMessageTypesClasses::REQUEST | stun::StunMessageTypesMethods::BINDING, //0b0000000000000001
        BINDING_SUCCESS_RESPONSE = stun::StunMessageTypesClasses::SUCCESS_RESPONSE | stun::StunMessageTypesMethods::BINDING, //0b0000000100000001
    };
}

#endif //STUN_MESSAGE_TYPES_H