#include "Connection.h"
#include <string>
#include <thread>
#include "Utils/Log.h"
#include "Utils/Utils.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
char buffer[1500000];//������
Connection::Connection(int clientFd):mClientFd(clientFd){
    LOGI("clientFd=%d", clientFd);
}
Connection::~Connection() {
    LOGI("clientFd=%d", mClientFd);
    closesocket(mClientFd);
}
int Connection::start() {//��ʼ����
    char rec_buf[2000] = { 0 };//������
    int rec_buf_size = recv(mClientFd, rec_buf, 2000, 0);
    LOGI("bufRcvSize=%d,bufRecv=%s", rec_buf_size, rec_buf);
    //ͳһ��Դ��ʶ��,��ʶts�ļ���m3u8�ļ���
    char uri[100] = { 0 };// /, /index0.ts, /index1.ts ,,,
    
    /*��׼���з�ģ��*/
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
    //�зֵ���Ƶ�ļ������ڴ�
    std::string filename = "../test/test_pieces" + std::string(uri);
    
    /*        �ļ���д        */
    FILE* fp;
    //��ֻ��&�����Ʒ�ʽ���ļ�
    fp = fopen(filename.c_str(), "rb");
    //��ʧ��
    if (!fp) {
        LOGE("Failed to open %s", filename.c_str());
        return -1;
    }

    //�Զ����Ķ�������û��ʹ��strlen()����sizeof()�󳤶Ⱥʹ�С
    int bufLen = fread(buffer, 1, sizeof(buffer), fp);

    LOGI("bufLen=%d", bufLen);
    //��׳��
    if (fp) {
        fclose(fp);
    }

    /*httpͷ��Ϣ*/
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
    else {//��Ƭ
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
