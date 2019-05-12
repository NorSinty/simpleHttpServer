#include "Socket.h"

int     listenfd;
int     port;
char*   root;

void loadConf();

int main()
{
    loadConf();

    listenfd = createSocket();
    bindSocket(listenfd, port);
    listenScoket(listenfd);

    printf("服务器信息:\n");
    printf("\t端口：\t%d\n", port);
    printf("\t根目录：\t%s\n", root);
    printf("\n");

    acceptConnection(listenfd, root);

    exit(0);
}


void loadConf()
{
    FILE *fp = fopen("conf.conf", "r");

    if (fp == NULL) {
        perror("无法读取配置文件");
        exit(-1);
    }

    char *currline = malloc(128);

    fscanf(fp, "%s %d", currline, &port);
    if (strcmp(currline, "port") != 0) {
        printf("读取端口号错误");
        free(currline);
        exit(-1);
    }

    root = malloc(128);

    fscanf(fp, "%s %s", currline, root);
    if (strcmp(currline, "root") != 0) {
        printf("读取根目录错误");
        free(currline);
        free(root);
        exit(-1);
    }

    free(currline);
}