#ifndef REDNET_UKQUEUE_H
#define REDNET_UKQUEUE_H

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#include "upoll.h"

namespace rednet 
{
    namespace network
    {
        class ukqueue : public upoll
        {
            public:
                ukqueue();
                ~ukqueue();

                bool reg(int sock, void *ud) override;
                void unReg(int sock) override;
                void write(int sock, void *ud, bool enable) override;
            protected:
                int wait__(struct pollEvent *e, int max = 64) override;
            protected:
                pollFD create__() override;
                void release__() override;
        };
    }
}


#endif


#endif



