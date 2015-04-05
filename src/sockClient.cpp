#include "sockClient.h"
#include <string.h>
#include <unistd.h>

ipcCliSock::ipcCliSock(){
    strncpy(server_filename,"/tmp/socket-server", sizeof("/tmp/socket-server"));
    strncpy(client_filename,"/tmp/socket-client", sizeof("/tmp/socket-client"));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, server_filename, strlen(server_filename));
    unlink(server_addr.sun_path);
    
    memset(&client_addr, 0, sizeof(client_addr));
    strncpy(client_addr.sun_path, client_filename, strlen(client_filename));
    client_addr.sun_family = AF_UNIX;
    unlink(client_addr.sun_path);

    if ((s = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0){
        perror("Client Socket");
    }

    /*int tr=1;
    // kill "Address already in use" error message
    if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }*/
    
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

ipcCliSock::~ipcCliSock(){
     close(s);
     //delete &server_addr, &client_addr;
}
