#ifndef DESERIALIZE_EXCEPTION_H
#define DESERIALIZE_EXCEPTION_H

#include <exception>

namespace krb
{
    class DeserializeException: public std::exception
    {
        public:
            virtual const char* what() const throw()
            {
                return "Deserializing failed";
            }
};
}

#endif //DESERIALIZE_EXCEPTION_H