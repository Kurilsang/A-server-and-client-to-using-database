#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void working(sockaddr_in addr, SOCKET& client);
void clientStart()
{
	char ip_server;
	//std::cout << "请输入您要连接的服务端的ip地址 > " << std::endl;
	//std::cin >> ip_server;
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		std::cout << "WSAStartup error:" << WSAGetLastError() << std::endl;
		return;
	}
	//创建SOCKET并绑定地址簇--------------------
	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	//addr.sin_addr.S_un.S_addr = inet_addr(&ip_server);
	addr.sin_addr.S_un.S_addr = inet_addr("10.66.180.213");
	addr.sin_port = htons(6969);
	//连接服务端
	int len = sizeof(sockaddr_in);
	int c = connect(client, (sockaddr*)&addr, len);
	send(client, "我链接了", strlen("我链接了"), 0);
	//接收并输出（正常相应工作）
	if (c == -1)
	{
		std::cout << "error" << std::endl;
	}
	else
	{
		std::cout << "连接成功" << std::endl;
	}
	working(addr, client);

	//关闭
	closesocket(client);
	WSACleanup();
}

void working(sockaddr_in addr,SOCKET& client) {
	while (1)
	{
		char buf[1024] = { 0 };
		char msg_send[1024] = { 0 };
		int len_recv = recv(client, buf, 1024, 0);//第三个参数必须要为常数
		char ip[1024];
		inet_ntop(AF_INET, &addr.sin_addr.S_un.S_addr, ip, sizeof(ip));
		if (len_recv > 0)
		{//正常情况
			std::cout << buf << std::endl;
			std::cout << ip << " > " << std::endl;
			std::cin >> msg_send;
			send(client, msg_send, strlen(msg_send), 0);
		}
		else {
			perror("连接好像出了点问题...");
			break;
		}
		
	}
}

int main()
{
	clientStart();

	return 0;
}