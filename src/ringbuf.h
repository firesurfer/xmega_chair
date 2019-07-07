#pragma once
#include <stdint.h>

template <unsigned s>
struct bits{
    enum{b=bits<(s>>1)>::b+1};
};
template <>
struct bits<1>{
    enum{b=1};
};
template <bool,typename A,typename B> struct if_;
template <typename A,typename B> struct if_<true,A,B>{typedef A type;};
template <typename A,typename B> struct if_<false,A,B>{typedef B type;};

template <unsigned minsize>
struct ringbuf_t{
    enum{
        BITS=bits<minsize>::b,
        SIZE=1<<BITS,
        MASK=SIZE-1,
    };
    typedef typename if_<BITS<=8,uint8_t,uint16_t>::type T;
    typedef uint8_t BOOL;

    uint8_t data[SIZE];
    T front;
    volatile T back;
    inline BOOL full(){return ((front+1)&MASK)==back;}
    inline BOOL empty(){return front==back;}
    inline BOOL push_back(uint8_t b){if(full())return 0;front=(front+1)&MASK;data[front]=b;return 1;}
    inline BOOL pop_front(uint8_t &b){if(empty())return 0;back=(back+1)&MASK;b=data[back];return 1;}//cache back with register variable, because interrupt
    inline void clear(){front=0;back=0;}
};
