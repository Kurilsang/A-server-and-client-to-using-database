#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void working(sockaddr_in addr, SOCKET& client);
void clientStart()
{
	char ip_server;
	//std::cout << "��������Ҫ���ӵķ���˵�ip��ַ > " << std::endl;
	//std::cin >> ip_server;
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		std::cout << "WSAStartup error:" << WSAGetLastError() << std::endl;
		return;
	}
	//����SOCKET���󶨵�ַ��--------------------
	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	//addr.sin_addr.S_un.S_addr = inet_addr(&ip_server);
	addr.sin_addr.S_un.S_addr = inet_addr("10.66.180.213");
	addr.sin_port = htons(6969);
	//���ӷ����
	int len = sizeof(sockaddr_in);
	int c = connect(client, (sockaddr*)&addr, len);
	send(client, "��������", strlen("��������"), 0);
	//���ղ������������Ӧ������
	if (c == -1)
	{
		std::cout << "error" << std::endl;
	}
	else
	{
		std::cout << "���ӳɹ�" << std::endl;
	}
	working(addr, client);

	//�ر�
	closesocket(client);
	WSACleanup();
}

void working(sockaddr_in addr,SOCKET& client) {
	while (1)
	{
		char buf[1024] = { 0 };
		char msg_send[1024] = { 0 };
		int len_recv = recv(client, buf, 1024, 0);//��������������ҪΪ����
		char ip[1024];
		inet_ntop(AF_INET, &addr.sin_addr.S_un.S_addr, ip, sizeof(ip));
		if (len_recv > 0)
		{//�������
			std::cout << buf << std::endl;
			std::cout << ip << " > " << std::endl;
			std::cin >> msg_send;
			send(client, msg_send, strlen(msg_send), 0);
		}
		else {
			perror("���Ӻ�����˵�����...");
			break;
		}
		
	}
}

int main()
{
	clientStart();

	return 0;
}