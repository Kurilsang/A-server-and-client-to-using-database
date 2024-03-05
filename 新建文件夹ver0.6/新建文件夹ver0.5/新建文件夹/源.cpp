
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
//用全局变量储存读取进使用的数据
std::unordered_map<std::string, std::string> hash;
std::unordered_map<std::string, std::list<std::string>> hash_list;

//class commands {
//public:
//	std::string command;//输入的指令存储在这
//	std::string key;//键值
//	std::string value;//k-v中的v
//	std::string msg;//一些文字信息
//	int start;//输出某范围的起始位置
//	int end;//输出某范围的结尾位置
//	std::string field;
//};

class commands {
public:
	char command[100];//输入的指令存储在这
	char key[100];//键值
	char value[100];//k-v中的v
	char msg[100];//一些文字信息
	int start[100];//输出某范围的起始位置
	int end[100];//输出某范围的结尾位置
	char field[100];
};

//list用来移动迭代器的函数
auto findListByNum(std::list<std::string>::iterator &it, int n) {//传入迭代器，下标索引（利用迭代器实现类似的概念）
	for (int i = 0; i < n; i++)
	{
		it++;
	}
	return it;
}
//list用迭代器来遍历的函数
auto printList(int num1, int num2, std::list<std::string>list)
{
	std::vector<std::string>rtn;//return来用的
	if (num1 <= num2)
	{
		//从前往后遍历
		auto it = list.begin();
		findListByNum(it, num1);//迭代器移动到起始位置
		for (int i = 0; i < num2 - num1; it++)
		{
			std::cout << *it << " ";
			rtn.push_back(*it);
		}
		std::cout << std::endl;
		return rtn;
	}
	else {
		//从后往前遍历
		auto it = list.begin();
		findListByNum(it, num2);//迭代器移动到起始位置
		for (int i = 0; i < num1 - num2; it--)
		{
			std::cout << *it << " ";
			rtn.push_back(*it);
		}
		std::cout << std::endl;
		return rtn;
	}
}



void reac_cout(std::string msg,sockaddr_in caddr) {//每次反应服务器都要后台输出，直接写一个函数复用
	//参数1为每次变化的文字，参数2传入客户端地址簇
	char ip[100];
	std::cout << "向" << inet_ntop(AF_INET, &caddr.sin_addr.S_un.S_addr, ip, sizeof(ip)) << ":" << ntohs(caddr.sin_port) << msg << std::endl;
}

void working(SOCKET& client,sockaddr_in caddr) {//传入客户端SOCK,客户端ip簇
	//反复接收信息并响应
	int flag = 0;//防止成功连接提示反复出现
	commands cmds;//类型名缩写,创建对象方便存储接收的数据
	while (1)
	{
		char ip[100];//用于后面返回ip传参
		//成功连接服务器端显示
		if (flag == 0)
		{
			std::cout << "客户端连接成功>" << inet_ntop(AF_INET, &caddr.sin_addr.S_un.S_addr, ip, sizeof(ip)) << ":" << ntohs(caddr.sin_port) << std::endl;
		}
		flag = 1;
		
		int len_recv = recv(client, (char*)&cmds, sizeof(commands), 0);
		if (len_recv > 0)
		{
			//正常响应
			//ping! pong!
			if (strcmp("ping",cmds.command)== 0)
			{
				reac_cout("返回指令pong", caddr);
				send(client, "pong", sizeof("pong"), 0);
				continue;//开始下次准备下次接收信息
			}
			//字符串类型
			if (strcmp("set", cmds.command) == 0)
			{
				reac_cout("返回set指令", caddr);
				auto iter = hash.find((std::string)cmds.key);
				if (iter != hash.end())
				{//存在同名key，添加失败
					std::cout << "存在同名key，添加失败" << std::endl;
					send(client, "存在同名key，添加失败", sizeof("存在同名key，添加失败"), 0);
				}
				else {//可添加
					hash.insert({ (std::string)cmds.key, (std::string)cmds.value });
					std::cout << "添加成功：" << cmds.key << ":" << cmds.value << std::endl;
					send(client, "添加成功", sizeof("添加成功"), 0);
				}
			}
			if (strcmp("get", cmds.command) == 0)
			{
				//传入一个key
				auto iter = hash.find((std::string)cmds.key);//这里需要string，所以结构体改成string，结果报错了？？？
				if (iter != hash.end())
				{
					//找到了
					reac_cout("返回get指令", caddr);
					send(client, iter->second.c_str(), sizeof(iter->second), 0);
					std::cout << iter->second.c_str() << std::endl;
					continue;
				}
				else {
					//没找到
					reac_cout("返回get指令", caddr);
					std::cout << "查无此key" << std::endl;
					send(client, "查无此key", sizeof("查无此key"), 0);
				}
			}
			if (strcmp("del", cmds.command) == 0)
			{
				reac_cout("返回del指令", caddr);
				auto iter = hash.find((std::string)cmds.key);
				if (iter != hash.end())
				{
					//发现有，可删除
					hash.erase((std::string)cmds.key);
					std::cout << "删除成功：" << cmds.key << std::endl;
					send(client, "删除成功：", sizeof("删除成功："), 0);
				}
				else {
					//没发现，操作失败
					std::cout << "未发现可删除的k-v" << std::endl;
					send(client, "未发现可删除的k-v", sizeof("未发现可删除的k-v"), 0);
				}
				
			}
			//list相关指令
			if (strcmp("range", cmds.command) == 0)
			{//这里会传入一个key和2个数字
				reac_cout("返回指令range", caddr);
				auto iter = hash_list.find((std::string)cmds.key);
				if (iter != hash_list.end())
				{//这个key存在
					printList(*cmds.start, *cmds.end, iter->second);
				}
				else {//没有这个key
					std::cout << "未发现此k-v" << std::endl;
					send(client, "未发现此k-v", sizeof("未发现此k - v"), 0);
				}
			}
		}
		else if (len_recv == 0)
		{
			std::cout << "已断开与客户端的连接" << std::endl;
			closesocket(client);
		}
		else{//接收异常
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
	clients.resize(1024);
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
		std::thread th(working, std::ref(clients[i]),addr_c);//要引用调用到时候要用ref，否则报错
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