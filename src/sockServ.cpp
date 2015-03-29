#include "sockServ.h"

ipcSerSock::ipcSerSock(){
    server_filename = "/tmp/socket-server";
    client_filename = "/tmp/socket-client";

    strncpy(client_addr.sun_path, client_filename, strlen(client_filename));
    client_addr.sun_family = AF_UNIX;

    if ((s = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0){
        perror("Server Socket");
    }

    if(bind(s, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
	    perror("Server Bind");
    }

    /*if (listen(s, 5) < 0){
        return false;
    }*/
}

char* ipcSerSock::sockRecv(){
     char dataPtr[1024];
     /*if (recv(s, dataPtr, sizeof(*dataPtr), 0) < 0){
          return false;
     }*/
     if(read(s, dataPtr, sizeof(dataPtr)) < 0){
        return false;
     }
     return dataPtr;
}

bool ipcSerSock::sockClose(){
     close(s);
     return true;
}
