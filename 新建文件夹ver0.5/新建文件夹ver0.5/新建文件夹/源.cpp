#include <iostream>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

void working(SOCKET& client) {//传入客户端SOCK,
	//反复接收信息并响应
	while (1)
	{
		char msg[1024] = { 0 };
		int len_recv = recv(client, msg, sizeof(msg), 0);
		if (len_recv > 0)
		{
			//正常响应
			//ping! pong!
			if (strcmp(msg, "ping") == 0)
			{
				std::cout << "返回指令pong,len" << len_recv << std::endl;
				send(client, "pong", strlen("pong"), 0);
				continue;//开始下次准备下次接收信息
			}
		}
		else if (len_recv == 0)
		{
			std::cout << "已断开与客户端的连接" << std::endl;
			closesocket(client);
		}
		else {//接收异常
			perror("recv");
			closesocket(client);
			return;
		}
	}
}

void serverStart()
{
	//启动WSA前置-------------------------
	WSAData wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{//如果启动异常输出该错误并直接退出
		std::cout << "WSAStartup error:" << GetLastError() << std::endl;
		return;
	}
	//创建SOCKET----------------------------
	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

	if (server == INVALID_SOCKET)
	{
		//发现创建失败
		std::cout << "SOCKET create error:" << GetLastError() << std::endl;
		WSACleanup();
		return;
	}
	//bind && listen-------------------
	//创建地址的结构体
	sockaddr_in addr_s;
	//bind绑定
	addr_s.sin_family = AF_INET;
	addr_s.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");//本机所有ip地址
	addr_s.sin_port = htons(6969);//端口
	int b = bind(server, (sockaddr*)&addr_s, sizeof(sockaddr_in));
	listen(server, 5);//一瞬间最多5个
	//accept-----------------等待连接
	//获取客户端ip与端口
	sockaddr_in addr_c;
	addr_c.sin_family = AF_INET;
	int len_c = sizeof(sockaddr_in);
	std::cout << "accepting..." << std::endl;//方便调试
	//反复接收查看有无多个客户端
	std::vector<SOCKET> clients;
	clients.resize(5);
	int i = 0;
	while (1)
	{//客户端socket如果仅用临时变量创建似乎会冲突，导致双开时候，先开的一个客户端失效
		i++;
		clients[i] = accept(server, (sockaddr*)&addr_c, &len_c);
		if (clients[i] == INVALID_SOCKET)
		{
			std::cout << "client SOCKET create error:" << GetLastError() << std::endl;
			WSACleanup();
			break;
		}
		std::cout << "连接上了" << std::endl;
		
		char buf[1024] = { 0 };
		int ret = send(clients[i], "you have been connected!", strlen("you have been connected!"), 0);
		recv(clients[i], buf, 1024, 0);
		std::cout << buf << std::endl;
		std::thread th(working, std::ref(clients[i]));//要引用调用到时候要用ref，否则报错
		th.detach();
	}

	//对命令进行响应
	
	//结束,关闭服务
	closesocket(server);
	WSACleanup();
}


int main()
{
	serverStart();

	return 0;
}