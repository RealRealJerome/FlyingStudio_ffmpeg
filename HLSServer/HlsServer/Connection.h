#pragma once
#include <vector>
#include <mutex>
#include <queue>
class Connection {
public://����&����
	Connection(int clientFd);
	~Connection();
	int start();//��ʼ����
private:
	int mClientFd;
};