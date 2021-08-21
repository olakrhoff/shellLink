#ifndef NETWORK_BYTE_ARRAY_H
#define NETWORK_BYTE_ARRAY_H

#define START_BYTES 8

#include <iostream>

#include "types.h"
#include "IllegalArgumentException.h"

namespace krb
{
    class NetworkByteArray
    {
        private:
            int size {START_BYTES}, head {0}, tail {0};
            unsigned char* byte_array {nullptr};

        public:
            NetworkByteArray()
            {
                this->byte_array = (unsigned char*)malloc(START_BYTES);
            }

            NetworkByteArray(unsigned char* new_value, int length)
            {
                this->byte_array = (unsigned char*)malloc(START_BYTES);
                addByteArray(new_value, length);
            }

            ~NetworkByteArray()
            {
                if (this->byte_array != nullptr)
                    free(this->byte_array);
            }

            void addByte(unsigned char new_value)
            {
                this->expand(); //Check if we need to expand the array, if so it will
                this->byte_array[this->tail++] = new_value;
            }

            void addUInt16(krb::UInt16 new_value)
            {
                this->expand(2); //UInt16 = 2 byte
                
                for (int i = 1; i >= 0; i--)
                    this->byte_array[this->tail++] = (unsigned char)(new_value >> i * 8);
            }

            void addUInt32(krb::UInt32 new_value)
            {
                this->expand(4); ////UInt32 = 4 byte

                for (int i = 3; i >= 0; i--)
                    this->byte_array[this->tail++] = (unsigned char)(new_value >> i * 8);
            }

            void addInt32(krb::Int32 new_value)
            {
                this->expand(4); ////UInt32 = 4 byte

                for (int i = 3; i >= 0; i--)
                    this->byte_array[this->tail++] = (unsigned char)(new_value >> i * 8);
            }

            void addByteArray(unsigned char* new_value, int length)
            {
                this->expand(length);
                for (int i = 0; i < length; i++)
                    this->byte_array[this->tail++] = new_value[i];
            }

            //Creates the byte array ready to be sent over a socket
            //This method ALLOCATES MEMORY which has to be free()'ed later
            //This is why the method recuires a pointer by reference as a parameter
            void getByteArray(unsigned char* &ptr)
            {
                if (this->tail <= 0)    
                    ptr = nullptr;
                
                ptr = (unsigned char*)malloc(this->tail);
                memcpy(ptr, this->byte_array, this->tail);
            }

            krb::UInt8 getNextByte()
            {
                if (this->head <= this->tail)
                    return this->byte_array[head++];
                
                throw IllegalArgumentException("NetworkByteArray::getNextByte()");
            }

            void getNextNBytes(unsigned char *&ptr, int length)
            {
                if (ptr != nullptr) //This is to not have a memory leak
                    free(ptr);

                ptr = (unsigned char *)malloc(length);
                
                for (int i = 0; i < length; i++)
                    ptr[i] = this->getNextByte();
            }

            krb::UInt16 getNextUInt16()
            {
                krb::UInt16 return_value {};

                for (int i = 1; i >= 0; i--)
                    return_value = return_value | ((unsigned char)this->byte_array[this->head++] << i * 8);

                return return_value;
            }

            krb::UInt32 getNextUInt32()
            {
                krb::UInt32 return_value {};

                for (int i = 3; i >= 0; i--)
                    return_value = return_value | ((unsigned char)this->byte_array[this->head++] << i * 8);

                return return_value;
            }

            krb::Int32 getNextInt32()
            {
                krb::Int32 return_value {};

                for (int i = 3; i >= 0; i--)
                    return_value = return_value | ((unsigned char)this->byte_array[this->head++] << i * 8);

                return return_value;
            }

            //Returns the number of elements in the Array
            int getSize()
            {
                return this->tail;
            }

            //Resets the object to its start state
            void reset()
            {
                this->size = START_BYTES;
                this->head = 0;
                this->tail = 0;
                free(this->byte_array);
                this->byte_array = (unsigned char *)malloc(START_BYTES);
            }

            //This works as it does on arrays
            unsigned char &operator[](int index)
            {
                if (index >= tail)
                    throw IllegalArgumentException("NetworkByteArray::operator[]");
                return this->byte_array[index];
            }


        private:
            void expand(int adding = 1, int new_size = 0)
            {
                if (new_size == 0)
                    new_size = this->size;
                
                //We consider if we are expanding enough
                if (adding > (new_size - this->tail))
                {
                    this->expand(adding, new_size << 1); //Recursivly expand the array to a big enough size
                    return;
                }
                if (new_size == this->size) //If this is true then there is no need to switch the array
                    return;
                
                unsigned char* new_byte_array = (unsigned char*)malloc(new_size);

                memcpy(new_byte_array, this->byte_array, new_size);
                this->size = new_size;
                free(this->byte_array);
                this->byte_array = new_byte_array;
            }
    };
}

#endif //NETWORK_BYTE_ARRAY_H