#include <iostream>
#include <list>

std::list<std::string>::iterator function(std::list<std::string>::iterator it, int n) {
	for (int i = 0; i < n; i++)
	{
		it++;
	}
	return it;
}

std::list<std::string> list;
int main() {
	list.push_back("1");
	list.push_back("2");
	list.push_back("3");
	list.push_back("4");
	list.push_back("5");
	auto it = list.begin();
	std::cout << *function(it, 4);
	return 0;
}