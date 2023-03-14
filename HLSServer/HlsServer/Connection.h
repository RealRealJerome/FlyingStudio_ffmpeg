#pragma once
#include <vector>
#include <mutex>
#include <queue>
class Connection {
public://构造&析构
	Connection(int clientFd);
	~Connection();
	int start();//开始连接
private:
	int mClientFd;
};