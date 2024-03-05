
#include <iostream>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <winnt.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
//��ȫ�ֱ��������ȡ��ʹ�õ�����
std::unordered_map<std::string, std::string> hash;
std::unordered_map<std::string, std::list<std::string>> hash_list;

//class commands {
//public:
//	std::string command;//�����ָ��洢����
//	std::string key;//��ֵ
//	std::string value;//k-v�е�v
//	std::string msg;//һЩ������Ϣ
//	int start;//���ĳ��Χ����ʼλ��
//	int end;//���ĳ��Χ�Ľ�βλ��
//	std::string field;
//};

class commands {
public:
	char command[100];//�����ָ��洢����
	char key[100];//��ֵ
	char value[100];//k-v�е�v
	char msg[100];//һЩ������Ϣ
	int start[100];//���ĳ��Χ����ʼλ��
	int end[100];//���ĳ��Χ�Ľ�βλ��
	char field[100];
};

//list�����ƶ��������ĺ���
auto findListByNum(std::list<std::string>::iterator &it, int n) {//������������±����������õ�����ʵ�����Ƶĸ��
	for (int i = 0; i < n; i++)
	{
		it++;
	}
	return it;
}
//list�õ������������ĺ���
auto printList(int num1, int num2, std::list<std::string>list)
{
	std::vector<std::string>rtn;//return���õ�
	if (num1 <= num2)
	{
		//��ǰ�������
		auto it = list.begin();
		findListByNum(it, num1);//�������ƶ�����ʼλ��
		for (int i = 0; i < num2 - num1; it++)
		{
			std::cout << *it << " ";
			rtn.push_back(*it);
		}
		std::cout << std::endl;
		return rtn;
	}
	else {
		//�Ӻ���ǰ����
		auto it = list.begin();
		findListByNum(it, num2);//�������ƶ�����ʼλ��
		for (int i = 0; i < num1 - num2; it--)
		{
			std::cout << *it << " ";
			rtn.push_back(*it);
		}
		std::cout << std::endl;
		return rtn;
	}
}



void reac_cout(std::string msg,sockaddr_in caddr) {//ÿ�η�Ӧ��������Ҫ��̨�����ֱ��дһ����������
	//����1Ϊÿ�α仯�����֣�����2����ͻ��˵�ַ��
	char ip[100];
	std::cout << "��" << inet_ntop(AF_INET, &caddr.sin_addr.S_un.S_addr, ip, sizeof(ip)) << ":" << ntohs(caddr.sin_port) << msg << std::endl;
}

void working(SOCKET& client,sockaddr_in caddr) {//����ͻ���SOCK,�ͻ���ip��
	//����������Ϣ����Ӧ
	int flag = 0;//��ֹ�ɹ�������ʾ��������
	commands cmds;//��������д,�������󷽱�洢���յ�����
	while (1)
	{
		char ip[100];//���ں��淵��ip����
		//�ɹ����ӷ���������ʾ
		if (flag == 0)
		{
			std::cout << "�ͻ������ӳɹ�>" << inet_ntop(AF_INET, &caddr.sin_addr.S_un.S_addr, ip, sizeof(ip)) << ":" << ntohs(caddr.sin_port) << std::endl;
		}
		flag = 1;
		
		int len_recv = recv(client, (char*)&cmds, sizeof(commands), 0);
		if (len_recv > 0)
		{
			//������Ӧ
			//ping! pong!
			if (strcmp("ping",cmds.command)== 0)
			{
				reac_cout("����ָ��pong", caddr);
				send(client, "pong", sizeof("pong"), 0);
				continue;//��ʼ�´�׼���´ν�����Ϣ
			}
			//�ַ�������
			if (strcmp("set", cmds.command) == 0)
			{
				reac_cout("����setָ��", caddr);
				auto iter = hash.find((std::string)cmds.key);
				if (iter != hash.end())
				{//����ͬ��key�����ʧ��
					std::cout << "����ͬ��key�����ʧ��" << std::endl;
					send(client, "����ͬ��key�����ʧ��", sizeof("����ͬ��key�����ʧ��"), 0);
				}
				else {//�����
					hash.insert({ (std::string)cmds.key, (std::string)cmds.value });
					std::cout << "��ӳɹ���" << cmds.key << ":" << cmds.value << std::endl;
					send(client, "��ӳɹ�", sizeof("��ӳɹ�"), 0);
				}
			}
			if (strcmp("get", cmds.command) == 0)
			{
				//����һ��key
				auto iter = hash.find((std::string)cmds.key);//������Ҫstring�����Խṹ��ĳ�string����������ˣ�����
				if (iter != hash.end())
				{
					//�ҵ���
					reac_cout("����getָ��", caddr);
					send(client, iter->second.c_str(), sizeof(iter->second), 0);
					std::cout << iter->second.c_str() << std::endl;
					continue;
				}
				else {
					//û�ҵ�
					reac_cout("����getָ��", caddr);
					std::cout << "���޴�key" << std::endl;
					send(client, "���޴�key", sizeof("���޴�key"), 0);
				}
			}
			if (strcmp("del", cmds.command) == 0)
			{
				reac_cout("����delָ��", caddr);
				auto iter = hash.find((std::string)cmds.key);
				if (iter != hash.end())
				{
					//�����У���ɾ��
					hash.erase((std::string)cmds.key);
					std::cout << "ɾ���ɹ���" << cmds.key << std::endl;
					send(client, "ɾ���ɹ���", sizeof("ɾ���ɹ���"), 0);
				}
				else {
					//û���֣�����ʧ��
					std::cout << "δ���ֿ�ɾ����k-v" << std::endl;
					send(client, "δ���ֿ�ɾ����k-v", sizeof("δ���ֿ�ɾ����k-v"), 0);
				}
				
			}
			//list���ָ��
			if (strcmp("range", cmds.command) == 0)
			{//����ᴫ��һ��key��2������
				reac_cout("����ָ��range", caddr);
				auto iter = hash_list.find((std::string)cmds.key);
				if (iter != hash_list.end())
				{//���key����
					printList(*cmds.start, *cmds.end, iter->second);
				}
				else {//û�����key
					std::cout << "δ���ִ�k-v" << std::endl;
					send(client, "δ���ִ�k-v", sizeof("δ���ִ�k - v"), 0);
				}
			}
		}
		else if (len_recv == 0)
		{
			std::cout << "�ѶϿ���ͻ��˵�����" << std::endl;
			closesocket(client);
		}
		else{//�����쳣
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
	clients.resize(1024);
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
		std::thread th(working, std::ref(clients[i]),addr_c);//Ҫ���õ��õ�ʱ��Ҫ��ref�����򱨴�
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