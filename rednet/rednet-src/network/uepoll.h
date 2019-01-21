#ifndef REDNET_UEPOLL_H
#define REDNET_UEPOLL_H

#ifdef __linux__

#include "upoll.h"

namespace rednet 
{
    namespace network
    {
        class uepoll : public upoll
        {
            public:
                uepoll();
                ~uepoll();

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


