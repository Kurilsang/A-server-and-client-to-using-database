//#include <iostream>
//#include <list>
//#include <unordered_map>
//
//std::list<std::string>::iterator function(std::list<std::string>::iterator it, int n) {
//	for (int i = 0; i < n; i++)
//	{
//		it++;
//	}
//	return it;
//}
//std::unordered_map<std::string, std::string>map;
//std::list<std::string> list;
//int main() {
//	list.push_back("1");
//	list.push_back("2");
//	list.push_back("3");
//	list.push_back("4");
//	list.push_back("5");
//	auto it = list.begin();
//	std::cout << *function(it, 4);
//	return 0;
//}
// unordered_map::find
#include <iostream>
#include <string>
#include <unordered_map>

int main()
{
    std::unordered_map<std::string, double> mymap = {
       {"mom",5.4},
       {"dad",6.1},
       {"bro",5.9} };

    std::string input;
    std::cout << "who? ";
    getline(std::cin, input);

    std::unordered_map<std::string, double>::const_iterator got = mymap.find(input);

    if (got == mymap.end())
        std::cout << "not found";
    else
        std::cout << got->first << " is " << got->second;

    std::cout << std::endl;

    return 0;
}