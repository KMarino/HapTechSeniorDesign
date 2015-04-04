#include "sockServ.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BYTESTORECV 1024

ipcSerSock::ipcSerSock(){
    strncpy(server_filename,"/tmp/socket-server", sizeof("/tmp/socket-server"));
    strncpy(client_filename,"/tmp/socket-client", sizeof("/tmp/socket-client"));

    strncpy(server_addr.sun_path, server_filename, strlen(server_filename));
    server_addr.sun_family = AF_UNIX;

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
     char* dataPtr = new char[BYTESTORECV]; //need to delete after receipt
     /*if (recv(s, dataPtr, sizeof(*dataPtr), 0) < 0){
          return false;
     }*/
     //pthread_t thread1, thread2;
     //pthread_mutex_t exitMutex;
     //pthread_mutex_init(&exitMutex,0);
     //dataPtr = (char*) pthread_create(&thread1, NULL, ipcSerSock::ptRecv, (void*)dataPtr);
     //pthread_cond_wait(&exitCond, &exitMutex);
     return dataPtr; 
}

void* ipcSerSock::ptRecv(void* dataPtr){
    read(s, (char*)dataPtr, (size_t)BYTESTORECV);
    printf("%c", (char*)dataPtr[0]);
    //pthread_cond_signal(&exitCond);
    return NULL;
}

ipcSerSock::~ipcSerSock(){
     close(s);
     delete &server_addr, &client_addr;
}
