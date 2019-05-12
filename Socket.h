#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/sysinfo.h>

#include "Recv.h"

#define MAXCONNECT 5
#define MAX_EVENT_NUM 1024

int createSocket()
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(-1);
    }
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    return sockfd;
}

int bindSocket(int sockfd, int port)
{
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
        perror("setsockopt error");
        exit(-1);
    }
    
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("bind error");
        exit(-1);
    }

    return 0;
}

int listenScoket(int sockfd)
{
    if (listen(sockfd, MAXCONNECT) != 0) {
        perror("listen error");
        exit(-1);
    }

    return 0;
}


void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    
    return;
}

/*
void acceptConnection(int listenfd, char *root)
{
    int clilen;
    int connfd;
    struct sockaddr_in cliaddr;

    int childpid;

    signal(SIGCHLD, sig_chld);

    while (1) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if ( (childpid = fork()) < 0) {
            perror("fork error");
        }
        else if (childpid == 0) {
            printf("\nnew connection:\n");
            close(listenfd);
            receive(connfd, root);
            exit(0);
        }
        close(connfd);
    }
}
*/

void process(int listenfd, char* root) 
{
    int connfd;
    int num;
    struct sockaddr_in cliaddr;
    int clilen;
    char buf[128];
    int i;

    struct epoll_event ev, events[MAX_EVENT_NUM];

    int epollfd = epoll_create(MAX_EVENT_NUM);
    
    ev.data.fd  = listenfd;
    ev.events   = EPOLLIN;
    //将listenfd添加到事件组种去
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("epoll_ctl error");
        exit(1);
    }

    while(1) {
        //printf("pid:%d is working\n", getpid());
        // 等待事件发生
        num = epoll_wait(epollfd, events, MAX_EVENT_NUM, -1);
        if (num == -1) {
            perror("epoll_wait error");
            continue;
        }
        for(i = 0; i < num; i++) {
            if ((events[i].data.fd == listenfd) && (events[i].events & EPOLLIN)) { // 有新的连接
                clilen = sizeof(cliaddr);
                if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) == -1) {
                    perror(buf);
                    continue;
                }

                //中间的fcntl是要获得connfd当前状态，然后与O_NONBLOCK或运算
                if (fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
                    continue;
                }

                ev.data.fd = connfd;
                ev.events = EPOLLIN;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
                    perror("epoll_ctl add error");
                    close(connfd);
                }
                
            } else if (events[i].events & EPOLLIN) {  
                connfd = events[i].data.fd;
                receive(connfd, root);               
                if (epoll_ctl(epollfd,EPOLL_CTL_DEL,connfd, &events[i]) < 0 ) {
                    perror("epoll_ctl del error");
                }
                close(connfd);
            } else if (events[i].events & EPOLLERR) {
                fprintf(stderr, "epoll error\n");
                close(connfd);
            }
        }
    }
}


void acceptConnection(int listenfd, char *root)
{
    // 多进程
    // int cpu_core_num;

    // signal(SIGCHLD, sig_chld);

    // cpu_core_num = get_nprocs();
    // printf("cpu core num: %d\n", cpu_core_num);

    // for (int i = 0; i < cpu_core_num; i++) {
    //     pid_t pid = fork();
    //     if (pid == 0) {    // 子进程
    //         process(listenfd, root);
    //         exit(0);
    //     }
    // }

    // while (1) {
    //     sleep(1);
    // }
    
    // 单进程
    process(listenfd, root);
    
}

