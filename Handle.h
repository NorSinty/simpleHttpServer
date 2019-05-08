#include "Send.h"
#include "Scan.h"

//计算文件的长度
int Content_Length(FILE* fp)
{
	int filesize = 0;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	rewind(fp);	

	return filesize;
}

//获取文件扩展名
int getExtension(char* filename, char* extension)
{
    int i;
    int len = strlen(filename);
    char *start = filename;
    for(i = 0; i < len; ++i) {
        if ( *(start + i) == '.')
            break;
    }

    i += 1;

    if (i > len) {
        printf("文件无扩展名\n");
        return -1;
    }

    strcpy(extension, (start+i));

    return 0;
}

//获取扩展名对应的ContentType
int getContentType(char* extension, char* content_type)
{
    FILE *fp = fopen("mime.types", "r");
    if (fp == NULL) {
        perror("无法打开mime.types文件");
        return -1;
    }

    char *ext  = (char*)malloc(10 * sizeof(char));
    char *type = (char*)malloc(100 * sizeof(char));
    while(fscanf(fp, "%s %s", type, ext) != EOF) {
        if (strcmp(ext, extension) == 0) {
            strcpy(content_type, type);
            return 0;
        }
    }
    printf("扩展不支持\n");
    return -1;
}

//处理GET请求
int handleHttpGET(int sockfd, char* message, char* root)
{
    char* filename     = (char*)malloc(200  * sizeof(char));
	char* path 	       = (char*)malloc(1000 * sizeof(char));
    char* extension    = (char*)malloc(10   * sizeof(char));
    char* content_type = (char*)malloc(100  * sizeof(char));
    FILE* fp;
    int length = 0;

    //获取访问的文件名
    if (scan(message, filename, 5, 200) <= 0) {
        goto err;
    }
    //如果文件名长度为0，则是访问index
    if (strlen(filename) == 0) {
		strcpy(filename, "index.html");
	}
    strcpy(path, root);
    strcat(path, filename);
    //printf("正在访问%s\n", path);

    if (getExtension(filename, extension) != 0) {     
        sendString(sockfd, "400 Bad Request");
        goto err;
    }

    if (getContentType(extension, content_type) != 0) {
        sendString(sockfd, "400 Bad Request");
        goto err;
    }

    fp = fopen(path, "rb");
    if (fp == NULL) {
        perror("Unable to open file");
        sendString(sockfd, "404 Not Found");
        goto err;
    }

    length = Content_Length(fp);
    if (length < 0) {
        printf("filesize is zero\n");
        goto err;
    }

    if (sendHeader(sockfd, "200 OK" , content_type, length) != 0) {
        sendString(sockfd, "500 Internal Server Error");
        goto err;
    }

    sendFile(sockfd, fp, length);
    free(filename);
    free(path);
    free(extension);
    free(content_type);
    return 0;

err:
    free(filename);
    free(path);
    free(extension);
    free(content_type);

    return -1;
}

int handleHttpHEAD(char *message)
{

}

int handleHttpPOST(char *message)
{

}