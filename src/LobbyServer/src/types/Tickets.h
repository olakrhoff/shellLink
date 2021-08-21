#ifndef TICKETS_H
#define TICKETS_H

#include "types.h"
#include "Ticket.h"

namespace krb
{
    class Tickets
    {
        private:
            int size {1};
            krb::UInt8 number_of_tickets {};
            krb::Ticket *tickets {nullptr};

        public:

            Tickets()
            {
                this->tickets = (krb::Ticket *)malloc(sizeof(krb::Ticket) * this->size);
            }

            ~Tickets()
            {
                if (this->tickets != nullptr)
                    free(tickets);
            }

            void addTicket(krb::Ticket &ticket)
            {
                this->expand();
                this->tickets[this->number_of_tickets++] = ticket;
            }

            void serialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                binaryDataBuffer.addByte(this->number_of_tickets);
                for (int i = 0; i < this->number_of_tickets; i++)
                    this->tickets[i].serialize(binaryDataBuffer);   
            }

            void deserialize(krb::NetworkByteArray &binaryDataBuffer)
            {
                //The first byte we get is the number of tickets the variable 'number_of_tickets'
                //will be correctly incremented after we have added all the tickets
                for (int i = 0; i < binaryDataBuffer.getNextByte(); i++)
                {
                    krb::Ticket temp;
                    temp.deserialize(binaryDataBuffer);
                    this->addTicket(temp);
                }
            }

            void print()
            {
                printf("Tickets:\nnumber_of_tickets: %d\n", this->number_of_tickets);
                for (int i = 0; i < this->number_of_tickets; i++)
                    this->tickets[i].print();
            }

        private:

            void expand()
            {
                if (this->number_of_tickets >= this->size)
                {
                    this->size = this->size << 1;
                    krb::Ticket *temp = this->tickets;

                    this->tickets = (krb::Ticket *)malloc(sizeof(krb::Ticket) * this->size);
                    memcpy(&this->tickets, &temp, this->size >> 1);
                    free(temp);
                }
                if (this->number_of_tickets >= this->size)
                    this->expand();
            }
    };
}


#endif //TICKETS_H