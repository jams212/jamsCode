#ifndef REDNET_ISOCKET_H
#define REDNET_ISOCKET_H

#include <uobject.h>
#include <stdint.h>

namespace rednet 
{
    namespace network
    {
        struct socketStat 
        {
            uint64_t rtts;
            uint64_t wtts;
            uint64_t recv;
            uint64_t write;
        };

        class tsocket : public uobject
        {
            public:
                tsocket(uint32_t opaque) : opaque__(opaque){}
                ~tsocket();

            public:
                int status;
            protected:
                int sock__;
                int id__;
                uint32_t opaque__;
                int protocol__;
                struct socketStat stat__;       
        };
    }
}

#endif


