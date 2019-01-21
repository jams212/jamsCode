#include "uepoll.h"

#ifdef __linux__

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace rednet 
{
    namespace network
    {
        uepoll::uepoll() : upoll()
        {
            fd__ = create__();
        }

        uepoll::~uepoll()
        {
            release__();
        }

        bool uepoll::reg(int sock, void *ud)
        {
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.ptr = ud;
            if (epoll_ctl(fd__, EPOLL_CTL_ADD, sock, &ev) == -1)
                return false;
            return true;
        }

        void uepoll::unReg(int sock)
        {
            epoll_ctl(fd__, EPOLL_CTL_DEL, sock, NULL);
        }

        void uepoll::write(int sock, void *ud, bool enable)
        {
            struct epoll_event ev;
            ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
            ev.data.ptr = ud;
            epoll_ctl(fd__, EPOLL_CTL_MOD, sock, &ev);
        }

        int uepoll::wait__(struct pollEvent *e, int max)
        {
            struct epoll_event ev[max];
            int n = epoll_wait(fd__, ev, max, -1);
            int i;
            for (i = 0; i < n; i++)
            {
                e[i].s = ev[i].data.ptr;
                unsigned flag = ev[i].events;
                e[i].write = (flag & EPOLLOUT) != 0;
                e[i].read = (flag & (EPOLLIN | EPOLLHUP)) != 0;
                e[i].error = (flag & EPOLLERR) != 0;
                e[i].eof = false;
            }
            return n;
        }

        pollFD uepoll::create__()
        {
            return epoll_create(1024);
        }

        void uepoll::release__()
        {
            close(fd__);
            fd__ = -1;
        }
    }
}

#endif 