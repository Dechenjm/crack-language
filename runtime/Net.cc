// Copyright 2010 Google Inc.

#include "Net.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif
#include <errno.h>
#include <signal.h>

#include <iostream>

#include "ext/Object.h"

using namespace std;

namespace {
    static void set_sockaddr_in(struct sockaddr_in *sa, 
                                crack::runtime::SockAddrIn *addr
                                ) {
        sa->sin_family = AF_INET;
        sa->sin_port = htons(addr->port);
        sa->sin_addr.s_addr = htonl(addr->addr);
    }
}

// our exported functions
namespace crack { namespace runtime {

// mirrors the _Constants class in crack.net

static Constants constants = {
    AF_UNIX,
    AF_LOCAL,
    AF_INET,
    AF_INET6,
    AF_IPX,
#ifndef __APPLE__
    AF_NETLINK,
    AF_X25,
    AF_AX25,
    AF_ATMPVC,
    AF_PACKET,
#endif
    AF_APPLETALK,
    SOCK_STREAM,
    SOCK_DGRAM,
    SOCK_SEQPACKET,
    SOCK_RAW,
    SOCK_RDM,
#ifndef __APPLE__
    SOCK_PACKET,
    SOCK_NONBLOCK,
    SOCK_CLOEXEC,
#endif
    SOL_SOCKET,
    SO_REUSEADDR,
    POLLIN,
    POLLOUT,
    POLLPRI,
    POLLERR,
    POLLHUP,
    POLLNVAL,
    INADDR_ANY
};

void SockAddrIn::init1(SockAddrIn *inst, uint8_t a, uint8_t b, uint8_t c, 
                       uint8_t d, 
                       unsigned int port0
                       ) {
    inst->family = AF_INET;
    inst->addr = makeIPV4(a, b, c, d);
    inst->port = port0;
}

void SockAddrIn::init2(SockAddrIn *inst, uint32_t addr0, unsigned int port0) {
    inst->family = AF_INET;
    inst->addr = addr0;
    inst->port = port0;
}

void TimeVal::init(TimeVal *inst, int32_t secs0, int32_t nsecs0) {
    inst->secs = secs0;
    inst->nsecs = nsecs0;
}

Constants *getConstants() { return &constants; }

uint32_t makeIPV4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (a << 24) | (b << 16) | (c << 8) | d;
}

int connect(int s, SockAddrIn *addr) {
    sockaddr_in sa;
    set_sockaddr_in(&sa, addr);
    return ::connect(s, (sockaddr *)&sa, sizeof(sa));
}

int bind(int s, SockAddrIn *addr) {
    sockaddr_in sa;
    set_sockaddr_in(&sa, addr);
    return ::bind(s, (sockaddr *)&sa, sizeof(sa));
}

int accept(int s, SockAddrIn *addr) {
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    socklen_t saSize = sizeof(sa);
    int newSock = ::accept(s, (sockaddr *)&sa, &saSize);
    if (newSock != -1) {
        addr->port = ntohs(sa.sin_port);
        addr->addr = ntohl(sa.sin_addr.s_addr);
    }

    return newSock;
}

int setsockopt_int(int fd, int level, int optname, int val) {
    return setsockopt(fd, level, optname, &val, sizeof(val));
}

pollfd *PollSet_create(unsigned int size) {
    return (pollfd *)calloc(size, sizeof(struct pollfd));
}

void PollSet_copy(struct pollfd *dst, struct pollfd *src, unsigned int size) {
    memcpy(dst, src, size * sizeof(struct pollfd));
}

void PollSet_destroy(struct pollfd *pollset) {
    free(pollset);
}

void PollSet_set(struct pollfd *set, unsigned int index, int fd, int events, 
                 int revents
                 ) {
    struct pollfd &elem = set[index];
    elem.fd = fd;
    elem.events = events;
    elem.revents = revents;
}

void PollSet_get(struct pollfd *set, unsigned int index,
                 PollEvt *outputEntry
                 ) {
    struct pollfd &elem = set[index];
    outputEntry->fd = elem.fd;
    outputEntry->events = elem.fd;
    outputEntry->revents = elem.fd;
}

// find the next poll entry that has an event in revents whose index is >= 
// index.  Makes it easy to iterate over the pollset.  Returns the index of 
// the item found, stores the entry info in outputEntry, -1 if no item was 
// found.
int PollSet_next(struct pollfd *set, unsigned int size, unsigned int index, 
                 PollEvt *outputEntry
                 ) {
    for (; index < size; ++index) {
        pollfd *elem = &set[index];
        if (elem->revents) {
            outputEntry->fd = elem->fd;
            outputEntry->events = elem->events;
            outputEntry->revents = elem->revents;
            return index;
        }
    }
    
    // not found.
    return -1;
}

int PollSet_poll(struct pollfd *fds, unsigned int nfds, TimeVal *tv,
                 sigset_t *sigmask
                 ) {
#ifdef __APPLE__
    int ready;
    sigset_t origmask;
    sigprocmask(SIG_SETMASK, sigmask, &origmask);
    if (tv) {
        ready = poll(fds, nfds, (tv->secs*1000)+(tv->nsecs/1000));
    } else {
        ready = poll(fds, nfds, 0);
    }
    sigprocmask(SIG_SETMASK, &origmask, NULL);
    return ready;
#else
    if (tv) {
        struct timespec ts = {tv->secs, tv->nsecs};
        return ppoll(fds, nfds, &ts, sigmask);
    } else {
        return ppoll(fds, nfds, 0, sigmask);
    }
#endif
}

sigset_t *SigSet_create() {
    return (sigset_t *)malloc(sizeof(sigset_t));
}

void SigSet_destroy(sigset_t *sigmask) {
    free(sigmask);
}

int SigSet_empty(sigset_t *sigmask) {
    return sigemptyset(sigmask);
}

int SigSet_fill(sigset_t *sigmask) {
    return sigfillset(sigmask);
}

int SigSet_add(sigset_t *sigmask, int signum) {
    return sigaddset(sigmask, signum);
}

int SigSet_del(sigset_t *sigmask, int signum) {
    return sigdelset(sigmask, signum);
}

int SigSet_has(sigset_t *sigmask, int signum) {
    return sigismember(sigmask, signum);
}

}} // namespace crack::runtime
