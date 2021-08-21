#ifndef KRB_KERBEROS_TIME_H
#define KRB_KERBEROS_TIME_H

#include <string>
#include <time.h>

#include "NetworkByteArray.h"

namespace krb
{
    class KerberosTime
    {
        private:
            krb::UInt16 _year {};
            krb::UInt16 _month {};
            krb::UInt16 _day {};
            krb::UInt16 _hour {};
            krb::UInt16 _minute {};
            krb::UInt16 _second {};

        public:
            KerberosTime()
            {
                std::time_t time = std::time(nullptr);
                tm* ltime = std::localtime(&time);
                
                this->_year = ltime->tm_year;
                this->_month = ltime->tm_mon;
                this->_day = ltime->tm_mday;
                this->_hour = ltime->tm_hour;
                this->_minute = ltime->tm_min;
                this->_second = ltime->tm_sec;
            }

            KerberosTime(krb::UInt16 year,
                            krb::UInt16 month,
                            krb::UInt16 day,
                            krb::UInt16 hour = 0,
                            krb::UInt16 minute = 0,
                            krb::UInt16 second = 0) : _year(year), _month(month), _day(day), _hour(hour), _minute(minute), _second(second)
            {
                //TODO: Error handling on valid dates
            }
            ~KerberosTime() {}

            void serialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                //TODO
            }

            void deserialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                KerberosTime object;
                //TODO
            }

            void print()
            {
                printf("KerberosTime:\nyear: %d, month: %d, day: %hu\nhour: %hu, minute: %hu, second: %hu\n", this->_year + 1900, this->_month + 1, this->_day, this->_hour, this->_minute, this->_second);
            }
    };
}
#endif //KRB_KERBEROS_TIME_H