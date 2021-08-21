#ifndef ILLEAGAL_ARGUMENT_EXCEPTION_H
#define ILLEAGAL_ARGUMENT_EXCEPTION_H

#include <exception>
#include <string>

namespace krb
{
    class IllegalArgumentException: public std::exception
    {
        private:
            std::string message = "Illegal argument failed: ";

        public:

            IllegalArgumentException(std::string message_value = "")
            {
                this->message.append(message_value);
            }

            virtual const char* what() const throw()
            {
                return this->message.data();
            }
};
}

#endif //ILLEAGAL_ARGUMENT_EXCEPTION_H