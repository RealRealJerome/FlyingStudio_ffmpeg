#include "Connection.h"
#include <string>
#include <thread>
#include "Utils/Log.h"
#include "Utils/Utils.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
char buffer[1500000];//缓冲区
Connection::Connection(int clientFd):mClientFd(clientFd){
    LOGI("clientFd=%d", clientFd);
}
Connection::~Connection() {
    LOGI("clientFd=%d", mClientFd);
    closesocket(mClientFd);
}
int Connection::start() {//开始连接
    char rec_buf[2000] = { 0 };//缓冲区
    int rec_buf_size = recv(mClientFd, rec_buf, 2000, 0);
    LOGI("bufRcvSize=%d,bufRecv=%s", rec_buf_size, rec_buf);
    //统一资源标识符,标识ts文件和m3u8文件等
    char uri[100] = { 0 };// /, /index0.ts, /index1.ts ,,,
    
    /*标准的切分模块*/
    const char* delim = "\n";
    char* line = strtok(rec_buf, delim);
    while (line) {
        if (strstr(line, "GET")) {
            if (sscanf(line, "GET %s HTTP/1.1\r\n", &uri) != 1) {
                LOGE("Failed to parse uri!");

            }
        }
        line = strtok(NULL, delim);
    }
    /*            */

    printf("uri=%s\n", uri);
    //切分的视频文件的所在处
    std::string filename = "../test/test_pieces" + std::string(uri);
    
    /*        文件读写        */
    FILE* fp;
    //以只读&二进制方式打开文件
    fp = fopen(filename.c_str(), "rb");
    //打开失败
    if (!fp) {
        LOGE("Failed to open %s", filename.c_str());
        return -1;
    }

    //对读出的二进制流没法使用strlen()或者sizeof()求长度和大小
    int bufLen = fread(buffer, 1, sizeof(buffer), fp);

    LOGI("bufLen=%d", bufLen);
    //健壮性
    if (fp) {
        fclose(fp);
    }

    /*http头信息*/
    char http_headers[2000];

    if (0 == strcmp("/index.m3u8", uri)) {
        sprintf(http_headers, "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: * \r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: application/vnd.apple.mpegurl; charset=utf-8\r\n"
            "Keep-Alive: timeout=30, max=100\r\n"
            "Server: hlsServer\r\n"
            "\r\n",
            bufLen);
    }
    else {//分片
        sprintf(http_headers, "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: * \r\n"
            "Connection: close\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: video/mp2t; charset=utf-8\r\n"
            "Keep-Alive: timeout=30, max=100\r\n"
            "Server: hlsServer\r\n"
            "\r\n",
            bufLen);
    }

    int http_headers_len = strlen(http_headers);
    LOGI("http_headers_len=%d", http_headers_len);

    send(mClientFd, http_headers, http_headers_len, 0);
    send(mClientFd, buffer, bufLen, 0);

    Sleep(10);

    return 0;

}
