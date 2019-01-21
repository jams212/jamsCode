#ifndef REDNET_UPOLL_H
#define REDNET_UPOLL_H


#include "uobject.h"
#include <errno.h>
#include <string.h>


#define POLL_EVENT_MAX 64

namespace rednet 
{
    namespace network
    {
        using pollFD = int;
        struct pollEvent
        {
            void *s;
            bool read;
            bool write;
            bool error;
            bool eof;
        };

        class upoll : public uobject
        {
            public:
                upoll() : event_n__(0), event_index__(0) { memset(events__, 0, sizeof(struct pollEvent) * POLL_EVENT_MAX); };
                virtual ~upoll() {};

                bool invalid() { return fd__ == -1; }
                virtual bool reg(int sock, void *ud) = 0;
                virtual void unReg(int sock) = 0;
                virtual void write(int sock, void *ud, bool enable) = 0;
                inline int wait()
                {
                    if (event_n__ == event_index__)
                    {
                        event_n__ = wait__(events__, POLL_EVENT_MAX);
                        event_index__ = 0;
                        if (event_n__ <= 0)
                        {
                            event_n__ = 0;
                            if (errno == EINTR)
                                return -2;
                            return -1;
                        }
                        return 2;
                    }
                    return 1;
                }

                inline pollEvent *getEvent()
                {
                    return &events__[event_index__++];
                }

                inline void decEvent()
                {
                    --event_index__;
                }
            protected:
                virtual int wait__(struct pollEvent *e, int max = 64) = 0;
            protected:
                virtual pollFD create__() = 0;
                virtual void release__() = 0;

            protected:
                pollFD         fd__;
                pollEvent      events__[POLL_EVENT_MAX];
                int event_n__;
                int event_index__;
        };
    }
}



#endif

