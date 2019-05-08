#include "Handle.h"
#include "Scan.h"

int getRequestType(char* message)
{
    if (strlen(message) <= 0)
        return -1;
    
    char *type = malloc(5);

    scan(message, type, 0, 5);

    if (strcmp(type, "GET") == 0)
        return 1;
    else if (strcmp(type, "HEAD") == 0)
        return 2;
    else if (strcmp(type, "POST") == 0)
        return 3;
    else
        return -1;

}

void receive(int connfd, char* root)
{
    char message[4096];

    if (recv(connfd, message, 4096, 0) == -1) {
        printf("recv error");
        close(connfd);
    }

    int type = getRequestType(message);

    if (type == 1) {
        handleHttpGET(connfd, message, root);
    }
    else if (type == 2) {
        //handleHttpHEAD(message);
    }
    else if (type == 3) {
        //handleHttpPOST(message);
    }
    else {
        //400 Bad Request
    }
}