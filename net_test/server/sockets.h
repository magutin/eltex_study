#ifndef SOCKETS_H
#define SOCKETS_H

#define S_PORT 60565	// порт источника
#define D_PORT 60564	// порт назначения


void* thread_tcp_sock();
void* thread_udp_sock();

#endif