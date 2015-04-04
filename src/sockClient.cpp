#include "sockClient.h"

ipcCliSock::ipcCliSock(){
    server_filename = "/tmp/socket-server";
    client_filename = "/tmp/socket-client";

    strncpy(server_addr.sun_path, server_filename, strlen(server_filename));
    server_addr.sun_family = AF_UNIX;

    if ((s = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0){
        perror("Client Socket");
    }

    if(bind(s, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0){
	    perror("Client Bind");
    }

    /*if (connect(s, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        return false;
    }*/
}

bool ipcCliSock::sockSend(char* dataPtr, int size){
     /*if (send(s, dataPtr, strlen(*dataPtr), 0) < 0){
          return false;
     }*/
     if(sendto(s, dataPtr, (size_t)size, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        return false;
    }
    return true;
}

bool ipcCliSock::~ipcCliSock(){
     close(s);
     delete server_addr, client_addr;
}
