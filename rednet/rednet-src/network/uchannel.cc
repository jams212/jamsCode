#include "uchannel.h"
#include "upoll.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

namespace rednet 
{
    namespace network
    {
  

        uchannel::uchannel() : rctrl__(-1),
                               sctrl__(-1),
                               cctrl__(0),
                               poll__(NULL)
        {
            FD_ZERO(&rfds__);
        }

        uchannel::~uchannel()
        {
            if (rctrl__ != -1)
            {
                if (poll__)
                    poll__->unReg(rctrl__);
                close(rctrl__);
                rctrl__ = -1;
            }

            if (sctrl__ != -1)
            {
                close(sctrl__);
                sctrl__ = -1;
            }
            poll__ = NULL;
        }

        bool uchannel::init(channelFunc f, upoll *poll)
        {
            int fd[2];
            poll__ = poll;
            if (pipe(fd))
            {
                fprintf(stderr, "ctrl: create socket pair failed.\n");
                return false;
            }

            if (!poll__->reg(fd[0], NULL))
            {
                close(fd[0]);
                close(fd[1]);
                return false;
            }

            rctrl__ = fd[0];
            sctrl__ = fd[1];
            cctrl__ = 1;

            assert(rctrl__ < FD_SETSIZE);
            cb__ = f;
            return true;
        }

        int uchannel::wait()
        {
            if (!cctrl__)
                return -2;
            if (!sel__())
            {
                cctrl__ = 0;
                return -2;
            }

            return disponse__();
        }

        void uchannel::rest()
        {
            cctrl__ = 1;
        }

        void uchannel::send(char *data, char cmd, int len)
        {
            uint8_t byte = (uint8_t)len;
            memcpy(data, &cmd, sizeof(char));
            memcpy(data + 1, &byte, sizeof(uint8_t));
            for (;;)
            {
                ssize_t n = write(sctrl__, data, len + 2);
                if (n < 0)
                {
                    if (errno != EINTR)
                    {
                        fprintf(stderr, "ctrl server : send command error %s.\n", strerror(errno));
                    }
                    continue;
                }
                assert(n == len + 2);
                return;
            }
        }

        bool uchannel::sel__()
        {
            struct timeval tv = {0, 0};
            int retval;

            FD_SET(rctrl__, &rfds__);
            retval = select(rctrl__ + 1, &rfds__, NULL, NULL, &tv);
            if (retval == 1)
                return true;
            return false;
        }

        int uchannel::disponse__()
        {
            int fd = rctrl__;
            uint8_t buffer[256];
            uint8_t header[2];
            recv__(fd, header, sizeof(header));
            int type = header[0];
            int len = header[1];
            recv__(fd, buffer, len);
            return cb__(type, buffer);
        }

        void uchannel::recv__(int fd, void *buffer, int sz)
        {
            for (;;)
            {
                int n = read(fd, buffer, sz);
                if (n < 0)
                {
                    if (errno == EINTR)
                        continue;
                    fprintf(stderr, "Channel : read pipe error %s.\n", strerror(errno));
                    return;
                }
                assert(n == sz);
                return;
            }
        }
    }
}