#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>
#include "../hooks.h"
#include "patches.h"
#include "../log.h"

#define LOG_TAG "MiniNetworkPatch"

bool allowNetwork = true;

#define netHook(func, ret, params, call)            \
STATIC_DL_HOOK_ADDR(h##func, func, ret, params) {   \
    if(allowNetwork)                                \
        return orig_h##func call;                   \
    errno = EACCES;                                 \
    return -1;                                      \
}

// How to build hooks: Copy all the functions from Android's socket.h.
// Find: __socketcall (.+?) (.+?)\((.+?)\);
// Repl: netHook(\n    $2,\n    $1,\n    ($3),\n    ($3)\n)
//
// Find: ` __`
// Replace: ` `
//
// Find: ((?:[\w*]+\s+)*[\w*]+)\s+(\w+)(?=,|\))
// Repl: $2
// (Only replace the second ones in each macro expansion.)
//
// Trim out any that are too short to hook:
// accept, send, recv...
//
// Make trigger function:
// Find: (?<=netHook\(\n    ).+?(?=,)
// Copy all occurrences, paste in function
// Find: (\w+)
// Repl: hook_h$1();
// Only replace in function obviously.

netHook(
	accept4,
	int,
	(int fd, struct sockaddr* _Nullable addr, socklen_t* _Nullable addr_length, int flags),
	(fd, addr, addr_length, flags)
)

netHook(
	bind,
	int,
	(int fd, const struct sockaddr* _Nonnull addr, socklen_t addr_length),
	(fd, addr, addr_length)
)

netHook(
	connect,
	int,
	(int fd, const struct sockaddr* _Nonnull addr, socklen_t addr_length),
	(fd, addr, addr_length)
)

netHook(
	getpeername,
	int,
	(int fd, struct sockaddr* _Nonnull addr, socklen_t* _Nonnull addr_length),
	(fd, addr, addr_length)
)

netHook(
	getsockname,
	int,
	(int fd, struct sockaddr* _Nonnull addr, socklen_t* _Nonnull addr_length),
	(fd, addr, addr_length)
)

netHook(
	getsockopt,
	int,
	(int fd, int level, int option, void* _Nullable value, socklen_t* _Nonnull value_length),
	(fd, level, option, value, value_length)
)

netHook(
	listen,
	int,
	(int fd, int backlog),
	(fd, backlog)
)

netHook(
	recvmmsg,
	int,
	(int fd, struct mmsghdr* _Nonnull msgs, unsigned int msg_count, int flags, const struct timespec* _Nullable timeout),
	(fd, msgs, msg_count, flags, timeout)
)

netHook(
	recvmsg, // THIS MIGHT BE TOO SMALL
	ssize_t,
	(int fd, struct msghdr* _Nonnull msg, int flags),
	(fd, msg, flags)
)

netHook(
	sendmmsg, // THIS MIGHT BE TOO SMALL
	int,
	(int fd, const struct mmsghdr* _Nonnull msgs, unsigned int msg_count, int flags),
	(fd, msgs, msg_count, flags)
)

netHook(
	sendmsg, // THIS MIGHT BE TOO SMALL
	ssize_t,
	(int fd, const struct msghdr* _Nonnull msg, int flags),
	(fd, msg, flags)
)

netHook(
	setsockopt,
	int,
	(int fd, int level, int option, const void* _Nullable value, socklen_t value_length),
	(fd, level, option, value, value_length)
)

netHook(
	shutdown,
	int,
	(int fd, int how),
	(fd, how)
)

netHook(
	socket,
	int,
	(int af, int type, int protocol),
	(af, type, protocol)
)

netHook(
	socketpair,
	int,
	(int af, int type, int protocol, int fds[_Nonnull 2]),
	(af, type, protocol, fds)
)

netHook(
	sendto,
	ssize_t,
	(int fd, const void* _Nonnull buf, size_t n, int flags, const struct sockaddr* _Nullable dst_addr, socklen_t dst_addr_length),
	(fd, buf, n, flags, dst_addr, dst_addr_length)
)

netHook(
	recvfrom,
	ssize_t,
	(int fd, void* _Nullable buf, size_t n, int flags, struct sockaddr* _Nullable src_addr, socklen_t* _Nullable src_addr_length),
	(fd, buf, n, flags, src_addr, src_addr_length)
)

void setupNetworkHooks() {
	LOGD("Applying Network patch");

//    hook_haccept4();
//    hook_hbind();
//    hook_hconnect();
//    hook_hgetpeername();
//    hook_hgetsockname();
//    hook_hgetsockopt();
//    hook_hlisten();
//    hook_hrecvmmsg();
//    hook_hrecvmsg();
//    hook_hsendmmsg();
//    hook_hsendmsg();
//    hook_hsetsockopt();
//    hook_hshutdown();
//    hook_hsocket();
//    hook_hsocketpair();
//    hook_hsendto();
//    hook_hrecvfrom();
}
