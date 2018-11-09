#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <iostream>
//#include <streambuf>
 
typedef std::istream_iterator<std::string> is;
typedef std::ostream_iterator<std::string> os;

std::vector<std::string> names;

struct CComparePredicate{
	inline bool operator()(const std::string &str){

		return str == "quit" ? true : (names.push_back(str), false);
	}
};
 
int main(){
    
//in.unsetf(std::ios::skipws);

std::find_if(is(std::cin), is(), CComparePredicate());
std::sort(names.begin(), names.end());
names.resize(std::distance(names.begin(), std::unique(names.begin(), names.end())));
std::copy(names.begin(), names.end(), os(std::cout, "\n"));

return 0;
}