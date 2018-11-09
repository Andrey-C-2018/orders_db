#include <string.h>

void strtrim(char *text, const size_t len){
size_t spaces, str_len;

if(!len) return;
str_len = len;

// left spaces
spaces = 0;
while(text[spaces] == ' ' && spaces < str_len)
	spaces++;

if(spaces == str_len){
	text[0] = '\0';
	return;
}
else strcpy(text, text + spaces);

// right spaces
str_len -= spaces;
spaces = 1;
while(text[str_len - spaces] == ' ' && str_len > spaces)
	spaces++;

text[str_len - spaces + 1] = '\0';
}

//*************************************************

string &trim(string &str){
size_t l_spaces, r_spaces, iKey_lspaces, iKey_rspaces;
string::iterator p, p_begin, p_end, p_rev;

if(str.empty()) return str;

p_begin = str.begin();
p_end = str.end();
p = p_begin;
p_rev = p_end - 1;
iKey_lspaces = 1;
l_spaces = 0;
r_spaces = 0;
while(p != p_end && p_rev != p_begin && iKey_lspaces){
	iKey_lspaces = (*p == ' ');
	l_spaces += iKey_lspaces;
	iKey_rspaces = (*p_rev == ' ');
	r_spaces += iKey_rspaces;
	p++;
	p_rev -= iKey_rspaces;
}
if(l_spaces == r_spaces && l_spaces == (str.size() - 1)){
	if(l_spaces || *p_begin == ' ') str.clear();
	return str;
}
if(l_spaces){
	p--;
	str.erase(p_begin, p);
}

iKey_rspaces = (r_spaces > 0);
p_begin = str.begin();
p_rev = str.end() - r_spaces - 1;
while(p_rev != p_begin && iKey_rspaces){
	iKey_rspaces = (*p_rev == ' ');
	r_spaces += iKey_rspaces;
	p_rev--;
}
if(r_spaces){
	p_rev += 2 - (int)((p_rev == p_begin) && iKey_rspaces);
	str.erase(p_rev, str.end());
}
return str;
}

//*************************************************

void strlcase(char * text, const size_t len){
size_t i;

if(!len) return;
for(i = 0; i < len; i++)
	text[i] = tolower(text[i]);
}
