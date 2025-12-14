#include <iostream>

using namespace std;

template <class T>
class Vector{
    private:
        T* data;
        int capacity; //Space of vector(Number of elements can be present in vector).
        int size; //Number of elements actually present in vector.

    public:
        Vector(): data(nullptr), capacity(0), size(0){}
        Vector(int initSize): data(new T[initSize]), capacity(initSize), size(0){}
        Vector(std::initializer_list<T> lst) : Vector(lst.size()){
            for(auto& itr : lst){
                push_back(itr);
            }
        }
        
        void push_back(T elem){
            if(capacity <= size){
                reserve(capacity == 0 ? 1 : capacity*2);
            }
            data[size++] = elem;
        }

        //For reserving the memory with new capcaity.
        void reserve(int newCapacity){
            if(newCapacity > capacity){
                T* temp = new T[newCapacity];
                for(int i=0;i<size;i++){
                    temp[i] = data[i];
                }
                delete[] data;
                data = temp;
                capacity = newCapacity;
            }
        }

        //Copy constructor: for copying another vector object to this.
        Vector(const Vector& other){
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for(int i=0;i<other.size;i++){
                data[i] = other.data[i];
            }
        }

        //Copy assignment operator, we can do cascading like a=b=c thats why we return Vector& as return type.
        Vector& operator=(const Vector& other){
            if(this != &other){
                capacity = other.capacity;
                size = other.size;
                
                //Here first the data is copied to the temporary memory because if below some exception happens
                //then the data shouldn't be lost and exception is thrown.
                T* temp = new T[capacity];

                for(int i=0;i<size;i++){
                    temp[i] = other.data[i];
                }
                delete[] data;
                data = temp;
            }
            /*
                Why dereference it with *

                The return type is Vector& (a reference, not a pointer)
                *this dereferences the pointer to get the actual object
                So *this has type Vector& (reference to the current object)

                Why not return by value?

                If you returned Vector (by value), it would create a temporary copy
                Less efficient; cascading would still work but slower
                Why not return a pointer?

                You could return this (a pointer), but returning a reference is more idiomatic in C++
                References are preferred for operator overloads (cleaner syntax: a = b = c instead of *a = *b = c)
            */
            return *this;
        }

        //Move copy constructor.
        Vector(Vector&& other){
            capacity = other.capacity;
            size = other.size;
            data = other.data;

            other.data = nullptr;
            other.capacity = 0;
            other.size = 0;
        }

        //Move assignment operator.
        Vector& operator=(Vector&& other){
            if(this!=other)
            {
                capacity = other.capacity;
                size = other.size;
                delete[] data;
                data = other.data;

                other.data = nullptr;
                other.capacity = 0;
                other.size = 0;
            }
            return *this;
        }

        Class iterator {
            private:
                T* elem;

            public:

        }

        iterator begin(){
            return iterator(data);
        }

        iterator end(){
            return iterator(data+size);
        }

};


int main(){
    return 0;
}