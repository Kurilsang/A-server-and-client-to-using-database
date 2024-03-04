#include <iostream>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

void working(SOCKET& client) {//����ͻ���SOCK,
	//����������Ϣ����Ӧ
	while (1)
	{
		char msg[1024] = { 0 };
		int len_recv = recv(client, msg, sizeof(msg), 0);
		if (len_recv > 0)
		{
			//������Ӧ
			//ping! pong!
			if (strcmp(msg, "ping") == 0)
			{
				std::cout << "����ָ��pong,len" << len_recv << std::endl;
				send(client, "pong", strlen("pong"), 0);
				continue;//��ʼ�´�׼���´ν�����Ϣ
			}
		}
		else if (len_recv == 0)
		{
			std::cout << "�ѶϿ���ͻ��˵�����" << std::endl;
			closesocket(client);
		}
		else {//�����쳣
			perror("recv");
			closesocket(client);
			return;
		}
	}
}

void serverStart()
{
	//����WSAǰ��-------------------------
	WSAData wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{//��������쳣����ô���ֱ���˳�
		std::cout << "WSAStartup error:" << GetLastError() << std::endl;
		return;
	}
	//����SOCKET----------------------------
	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

	if (server == INVALID_SOCKET)
	{
		//���ִ���ʧ��
		std::cout << "SOCKET create error:" << GetLastError() << std::endl;
		WSACleanup();
		return;
	}
	//bind && listen-------------------
	//������ַ�Ľṹ��
	sockaddr_in addr_s;
	//bind��
	addr_s.sin_family = AF_INET;
	addr_s.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");//��������ip��ַ
	addr_s.sin_port = htons(6969);//�˿�
	int b = bind(server, (sockaddr*)&addr_s, sizeof(sockaddr_in));
	listen(server, 5);//һ˲�����5��
	//accept-----------------�ȴ�����
	//��ȡ�ͻ���ip��˿�
	sockaddr_in addr_c;
	addr_c.sin_family = AF_INET;
	int len_c = sizeof(sockaddr_in);
	std::cout << "accepting..." << std::endl;//�������
	//�������ղ鿴���޶���ͻ���
	std::vector<SOCKET> clients;
	clients.resize(5);
	int i = 0;
	while (1)
	{//�ͻ���socket���������ʱ���������ƺ����ͻ������˫��ʱ���ȿ���һ���ͻ���ʧЧ
		i++;
		clients[i] = accept(server, (sockaddr*)&addr_c, &len_c);
		if (clients[i] == INVALID_SOCKET)
		{
			std::cout << "client SOCKET create error:" << GetLastError() << std::endl;
			WSACleanup();
			break;
		}
		std::cout << "��������" << std::endl;
		
		char buf[1024] = { 0 };
		int ret = send(clients[i], "you have been connected!", strlen("you have been connected!"), 0);
		recv(clients[i], buf, 1024, 0);
		std::cout << buf << std::endl;
		std::thread th(working, std::ref(clients[i]));//Ҫ���õ��õ�ʱ��Ҫ��ref�����򱨴�
		th.detach();
	}

	//�����������Ӧ
	
	//����,�رշ���
	closesocket(server);
	WSACleanup();
}


int main()
{
	serverStart();

	return 0;
}