#include "Socket.h"

int     listenfd;
int     port;
char*   root;

void loadConf();

int main(int argc, char** argv)
{
    loadConf();

    listenfd = createSocket();
    bindSocket(listenfd, port);
    listenScoket(listenfd);

    printf("Server info:\n");
    printf("\tPort：\t%d\n", port);
    printf("\tRoot：\t%s\n", root);
    printf("\n");

    acceptConnection(listenfd, root);

    exit(0);
}


void loadConf()
{
    FILE *fp = fopen("conf.conf", "r");

    if (fp == NULL) {
        perror("cannot load configure file");
        exit(-1);
    }

    char *currline = malloc(128);

    fscanf(fp, "%s %d", currline, &port);
    if (strcmp(currline, "port") != 0) {
        printf("load port error");
        free(currline);
        exit(-1);
    }

    root = malloc(128);

    fscanf(fp, "%s %s", currline, root);
    if (strcmp(currline, "root") != 0) {
        printf("load root error");
        free(currline);
        free(root);
        exit(-1);
    }

    free(currline);
}