#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int sendString(int sockfd, char* message)
{
    int n;
    n = send(sockfd, message, strlen(message), 0);
	return n;
}

int sendBinary(int sockfd, int*byte , int length)
{
	int n;
	n = send(sockfd, byte, length, 0);
	return n;
}

int sendHeader(int sockfd, char* StatusCode, char* ContentType, int totalsize)
{
    char *version           = "\r\nHTTP/1.1 ";
    char *server            = "\r\nServer: RoverWeb";
    char *content_type      = "\r\nContent-Type: ";
    char *content_length    = "\r\nContent-Length: ";
    char *date              = "\r\nDate: ";
    char *connection        = "\r\nConnection: Keep-Alive";
    char *newline           = "\r\n";

    char length[100];
    sprintf(length, "%i", totalsize);

    time_t rawtime;
    time(&rawtime);

    char *message = malloc((
        //
        strlen(version) +
        strlen(date) +
        strlen(server) +
        strlen(content_type) +
        strlen(content_length) +
        strlen(connection) +
        strlen(newline) +
        //
        strlen(StatusCode) +
        strlen(ContentType) +
        strlen(length) +
        28 +
        sizeof(char)) * 2);
    
    if (message != NULL) {
        strcpy(message, version);
        strcat(message, StatusCode);
        strcat(message, server);
        strcat(message, content_type);
        strcat(message, ContentType);
        strcat(message, content_length);
        strcat(message, length);
        strcat(message, connection);
        strcat(message, date);
        strcat(message, (char *)ctime(&rawtime));
        strcat(message, newline);

        sendString(sockfd, message);

        free(message);
        return 0;
    }

    return -1;
}

int sendFile(int sockfd, FILE* fp, int file_size)
{
    int byte;
    do {
        byte = fgetc(fp);
        sendBinary(sockfd, &byte, sizeof(char));
    }while(byte != EOF);
}