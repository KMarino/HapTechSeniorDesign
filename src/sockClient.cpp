#include "sockClient.h"

bool ipcCliSock::sockInit(){
    server_filename = "/tmp/socket-server";
    client_filename = "/tmp/socket-client";

    strncpy(server_addr.sun_path, server_filename, strlen(server_filename));
    server_addr.sun_family = AF_UNIX;

    if ((s = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0){
        return false;
    }

    if(bind(s, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0){
	return false;
    }

    if (connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        return false;
    }
}

bool ipcCliSock::sockSend(char* dataPtr){
     if (send(s, dataPtr, strlen(*dataPtr), 0) < 0){
          return false;
     }
}

bool ipcCliSock::sockClose(){
     close(s);
}
