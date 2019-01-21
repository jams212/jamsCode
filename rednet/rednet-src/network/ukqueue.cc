#include "ukqueue.h"


#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

namespace rednet
{
    namespace network
    {
        ukqueue::ukqueue() : upoll()
        {
            fd__ = create__();
        }

        ukqueue::~ukqueue()
        {
            release__();
        }

        bool ukqueue::reg(int sock, void *ud)
        {
            struct kevent ke;
            EV_SET(&ke, sock, EVFILT_READ, EV_ADD, 0, 0, ud);
            if (kevent(fd__, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
            {
                return false;
            }
            EV_SET(&ke, sock, EVFILT_WRITE, EV_ADD, 0, 0, ud);
            if (kevent(fd__, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
            {
                EV_SET(&ke, sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                kevent(fd__, &ke, 1, NULL, 0, NULL);
                return false;
            }
            EV_SET(&ke, sock, EVFILT_WRITE, EV_DISABLE, 0, 0, ud);
            if (kevent(fd__, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
            {
                UnRegister(sock);
                return false;
            }
            return true;
        }

        void ukqueue::unReg(int sock)
        {
            struct kevent ke;
            EV_SET(&ke, sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(fd__, &ke, 1, NULL, 0, NULL);
            EV_SET(&ke, sock, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
            kevent(fd__, &ke, 1, NULL, 0, NULL);
        }

        void ukqueue::write(int sock, void *ud, bool enable)
        {
            struct kevent ke;
            EV_SET(&ke, sock, EVFILT_WRITE, enable ? EV_ENABLE : EV_DISABLE, 0, 0, ud);
            if (kevent(fd__, &ke, 1, NULL, 0, NULL) == -1 || ke.flags & EV_ERROR)
            {
            }
        }

        int ukqueue::wait__(struct pollEvent *e, int max)
        {
            struct kevent ev[max];
            int n = kevent(fd__, NULL, 0, ev, max, NULL);

            int i;
            for (i = 0; i < n; i++)
            {
                e[i].s = ev[i].udata;
                unsigned filter = ev[i].filter;
                bool eof = (ev[i].flags & EV_EOF) != 0;
                e[i].write = (filter == EVFILT_WRITE) && (!eof);
                e[i].read = (filter == EVFILT_READ) && (!eof);
                e[i].error = (ev[i].flags & EV_ERROR) != 0;
                e[i].eof = eof;
            }

            return n;
        }

        pollFD ukqueue::create__()
        {
            return kqueue();
        }

        void ukqueue::release__()
        {
            close(fd__);
            fd__ = -1;
        }
    }
}


#endif