#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <locale.h>
#include <assert.h>

typedef std::string::iterator StrIterator;
typedef std::string::const_iterator StrConstIterator;
typedef std::pair<StrConstIterator, StrConstIterator> StrConstIteratorPair;

class CCsvFile{
	std::string buffer;
	const std::string all_delimiters;
	std::ifstream f;
		
	StrIterator find_first_of(StrIterator begin, StrIterator end,\
		const std::string &str_template){

			return std::find_first_of(begin, end, 
							str_template.begin(),\
							str_template.end());
	}
public:
	enum Errors{
		E_FILE_IS_EMPTY = 1,\
		E_WRONG_STRUCTURE = 2
	};
	CCsvFile(const char *path) : buffer(), all_delimiters("\\\","){
		
		f.open(path);
	}
	bool GetRecord(){
		bool eof = f.eof();

		if(!eof) getline(f, buffer);
		return !eof;
	}
	int GetItem(StrConstIterator start, StrConstIteratorPair &range){
		StrIterator p;
		bool key;
		char curr_delimiter;
		std::string templ = "\\";

		if(buffer.empty()) return E_FILE_IS_EMPTY;

		if(buffer[0] == '"')
			curr_delimiter = '"';
		else{
			if(buffer[0] == ','){
				range.first = range.second = buffer.end();
				return 0;
			}
			else
				curr_delimiter = ',';
		}
		
		templ += curr_delimiter;

		p = find_first_of(buffer.begin() + 1, buffer.end(), templ);
		if(p != buffer.end()) key = (*p == '\\');
		else key = false;

		while(key && p != buffer.end()){
			key = (*p == '\\' && *(p + 1) == '"');
			p = find_first_of(p + 1, buffer.end(), all_delimiters); // "\\\","
		}
		if(p == buffer.end()){
			if(curr_delimiter == '"')
				return E_WRONG_STRUCTURE;
			
			range.first = buffer.begin();
			range.second = buffer.end();
			return 0;
		}
		else{
			if(curr_delimiter == '"' && p + 1 != buffer.end() && *(p + 1) != ',')
				return E_WRONG_STRUCTURE;
		
			range.first = buffer.begin() + (int)(curr_delimiter == '"');
			range.second = p;

			return 0;
		}

		return E_WRONG_STRUCTURE;
	}
	virtual ~CCsvFile(){

		if(f) f.close();
	}
};

int main(){
setlocale(LC_ALL, "RUS");
CCsvFile csv("in.csv");
int ret_value;
bool Key = false;
StrConstIteratorPair range;

if(csv.GetRecord())
	Key = !(ret_value = csv.GetItem(range));
while(Key){
	std::copy(range.first, range.second, std::ostream_iterator<char>(std::cout, ""));
	std::cout<< '\n';
	Key = csv.GetRecord();
	ret_value = csv.GetItem(range);
	Key = Key && (!ret_value);
}
if(ret_value)
	std::cout<< "ret_value = "<< ret_value;
else
	std::cout<< "Готово. Hit <Enter>\n";
std::cin.sync();
std::cin.get();
return 0;
}
