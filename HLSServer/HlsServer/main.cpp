#include<cstdint>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include"Utils/Log.h"
#pragma comment(lib, "ws2_32.lib")
#include"Connection.h"
#include<thread>

int main(){
	//ffmpeg命令行生成m3u8切片
	//ffmpeg -i input.mp4 -c:v libx264 -c:a copy -f hls -hls_time 10 -hls_list_size 0 input/index.m3u8
	//	-hls_time n: 设置每片的长度，默认值为2,单位为秒
	//  -hls_list_size n:设置播放列表保存的最多条目，设置为0会保存有所片信息，默认值为5
	


	int port = 8080;
	LOGI("1-hlsServer http://127.0.0.1:%d/index.m3u8", port);
	//也就是http://127.0.0.1:8080/index.m3u8



	WSADATA wsaData;
	//启动WinSock库的使用
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LOGE("WSAStartup error");
		return -1;
	}
	//创建socket对象
	SOCKET serverFd;
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;//协议族
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//赋本机地址
	server_addr.sin_port = htons(port);//赋端口
	serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//ipv4协议族，流式套接字，TCP协议
	//SOCKADDR的缺陷在于把目标地址和端口混在一起存储
	if (bind(serverFd, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		LOGE("Failed to socket bind");
		return -1;
	}
	if (listen(serverFd, SOMAXCONN) < 0) {
		LOGE("Failed to socket listen");
		return -1;
	}


	while (true)
	{
		LOGI("waiting for new connection...");
		int len = sizeof(SOCKADDR);
		SOCKADDR_IN accept_addr;
		//第三个参数需要传入accept_addr的长度
		int clientFd = accept(serverFd, (SOCKADDR*)&accept_addr, &len);
		//const char* clientIp = inet_ntoa(accept_addr.sin_addr);

		if (clientFd == SOCKET_ERROR) {
			LOGE("Failed to accept connection");
			break;
		}
		LOGI("Discover new connection:clientFd=%d", clientFd);

			Connection conn(clientFd);
			conn.start();

	}

	closesocket(serverFd);
	return 0;
}