#ifndef REDNET_UCHANNEL_H
#define REDNET_UCHANNEL_H


#include <functional>
#include <sys/select.h>
#include <sys/time.h>

using namespace std;
namespace rednet 
{
    namespace network
    {
        class upoll;
        typedef function<int(int cmd, uint8_t *data)> channelFunc;
        class uchannel
        {
            public:
                uchannel();
                ~uchannel();

                bool init(channelFunc f, upoll *p);
                void send(char *data, char cmd, int len);
                
                int wait();
                void rest();
            private:
                bool sel__();
                int disponse__();
                void recv__(int fd, void *buffer, int sz);
                
            private:
                int rctrl__;
                int sctrl__;
                int cctrl__;
                upoll *poll__;
                fd_set rfds__;
                channelFunc cb__;
        };
    }
}



#endif

