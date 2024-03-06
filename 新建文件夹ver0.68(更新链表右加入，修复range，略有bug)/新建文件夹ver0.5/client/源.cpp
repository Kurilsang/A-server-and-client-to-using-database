#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <vector>

class commands {
public:
	char command[100];//输入的指令存储在这
	char key[100];//键值
	char value[100];//k-v中的v
	char msg[100];//一些文字信息
	int start;//输出某范围的起始位置
	int end;//输出某范围的结尾位置
	char field[100];

};


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
	send(client, "客户端连接了", sizeof("客户端连接了"), 0);
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

void working(sockaddr_in addr, SOCKET& client) {
	commands cmds;//创建对象方便发送
	while (1)
	{   //每次开始初始化，防止上次遗留
		strcpy(cmds.command, "未响应");
		strcpy(cmds.key, "未响应");
		strcpy(cmds.value, "未响应");
		strcpy(cmds.msg, "未响应");
		strcpy(cmds.field, "未响应");
		cmds.start = 0;
		cmds.end = 0;

		int len_recv = recv(client, (char*)&cmds, sizeof(commands), 0);//第三个参数必须要为常数
		char ip[1024];
		inet_ntop(AF_INET, &addr.sin_addr.S_un.S_addr, ip, sizeof(ip));
		if (len_recv > 0)
		{//正常情况
			std::cout << cmds.msg << std::endl;
			std::cout << ip << " > ";
			std::cin >> cmds.command;
			////command为ping
			//if (cmds.command.compare("ping") == 0)
			//{
			//	send(client, (char*)&cmds, sizeof(commands), 0);
			//}
			////command为字符串类型
			//if (cmds.command.compare("get") == 0)
			//{//这个分支还有一个参数
			//	std::cin >> cmds.key;
			//	send(client, (char*)&cmds, sizeof(commands), 0);
			//}
			//command为ping
			if (strcmp("ping", cmds.command) == 0)
			{
				send(client, (char*)&cmds, sizeof(commands), 0);
			}
			//哈希表操作------------------
			else if (strcmp("set", cmds.command) == 0)
			{
				std::cin >> cmds.key;
				std::cin >> cmds.value;
				send(client, (char*)&cmds, sizeof(commands), 0);
			}
			//command为字符串类型
			else if (strcmp("get", cmds.command) == 0)
			{//这个分支还有一个参数
				std::cin >> cmds.key;
				send(client, (char*)&cmds, sizeof(commands), 0);
			}
			else if (strcmp("del", cmds.command) == 0)
			{
				std::cin >> cmds.key;
				send(client, (char*)&cmds, sizeof(commands), 0);
			}
			//双向链表相关--------------
			else if (strcmp("lpush", cmds.command) == 0)
			{
				std::cin >> cmds.key;
				std::cin >> cmds.value;
				send(client, (char*)&cmds, sizeof(commands), 0);
			}
			else if (strcmp("rpush", cmds.command) == 0)
			{
				std::cin >> cmds.key;
				std::cin >> cmds.value;
				send(client, (char*)&cmds, sizeof(commands), 0);
			}
			else if (strcmp("range", cmds.command) == 0)
			{
				std::cin >> cmds.key;
				std::cin >> cmds.start;
				std::cin >> cmds.end;
				send(client, (char*)&cmds, sizeof(commands), 0);
				char flag[10] = { 0 };//判断一下到底有没有这个key
				recv(client, flag, sizeof(flag), 0);
				if (strcmp("1", flag) == 0)
				{
					if (cmds.start <= cmds.end)
					{
						for (int i = 0; i <= cmds.end - cmds.start; i++)
						{
							char buf[1024] = { 0 };
							recv(client, buf, sizeof(buf), 0);
							std::cout << buf << " ";
						}
						std::cout << std::endl;
					}
					else if (cmds.start > cmds.end)
					{
						for (int i = 0; i <= cmds.start - cmds.end; i++)
						{
							char buf[1024] = { 0 };
							recv(client, buf, sizeof(buf), 0);
							std::cout << buf << " ";
						}
						std::cout << std::endl;
					}
				}
				else {
					continue;
				}
			}
			else {
				std::cout << "未知的指令" << std::endl;
				//为了防止出现服务端未发送而导致停止在recv，发送ping
				strcpy(cmds.command, "ping");
				send(client, (char*)&cmds, sizeof(commands), 0);
				continue;
			}
		}
		else {
			perror("连接好像出了点问题...");
			return;
		}

	}
}

int main()
{
	clientStart();

	return 0;
}