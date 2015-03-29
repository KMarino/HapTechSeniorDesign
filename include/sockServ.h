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
	bool sockClose();
private:
	int s, t, len;
	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;
	char * server_filename;
	char * client_filename;
};
