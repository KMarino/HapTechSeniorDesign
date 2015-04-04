#ifndef SOCK_SERV_H
#define SOCK_SERV_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

class ipcSerSock{
public:
	ipcSerSock();
	char* sockRecv();
	~ipcSerSock();
private:
	int s, t, len;
	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;
	char server_filename[1024];
	char client_filename[1024];
};

#endif //SOCK_SERV_H
