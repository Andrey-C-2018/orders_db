#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>

using namespace std;

const char monthes_names[12][25] = {"Січень", "Лютий", "Березень", \
				"Квітень" ,"Травень" ,"Червень", \
				"Липень", "Серпень", "Вересень", \
				"Жовтень", "Листопад", "Грудень"};

const size_t count_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const char days[7][5] = {"Пн", "Вт" ,"Ср" ,"Чт" ,"Пт" ,"Сб" ,"Нд"};

int main(){
ofstream out;

out.open("calendar.html", std::ios::out | std::ios::trunc);
if(!out){
	std::cerr<< "File cannot be created"<< std::endl;
	return 1;
}

out<< "<html>"<< std::endl;
out<< "<head>"<< std::endl;
out<< "<meta charset=utf-8 />"<< std::endl;
out<< "</head>"<< std::endl;
out<< "<body>"<< std::endl;

out<< "<table border=0> <tr> "<< std::endl;

int day = 0;
size_t month = 0;

while(month < 12){
	size_t cols_count = ceil(((float)(abs(day)) + (float)(count_days[month])) / 7);
	int next_month_day = ((size_t)abs(day) + count_days[month]) % 7;
	next_month_day = -next_month_day;

	out<< "<td style=\"font-family:PF Centro Sans Pro; ";
	out<< "color:#286E28\"><p style=\"text-align:center\"><strong>"<< monthes_names[month];
	out<< "</strong></p>"<< std::endl;

	out<< "<table border=0>"<< std::endl;
	for(size_t i = 0; i < 7; ++i){
		out<< " <tr align=\"right\" ";
		out<< "style=\"font-family:PF Centro Sans Pro\"> ";
		out<< "<td style=\"font-size:10pt; color:";
		if(i == 6)
			out<< "red\">";
		else
			out<< "#286E28\">";
		out<< "<b>"<< days[i]<< "</b></td>";

		size_t col = 0;
		for(int j = day; col < cols_count; j += 7, ++col){
			char cell_value[10];
			if(j < 0 || j + 1 > (int)count_days[month])
				strcpy(cell_value, "&nbsp;");
			else{
				if(j + 1 >= 10){
					*cell_value = '0' + (j + 1)/10;
					*(cell_value + 1) = '0' + (j + 1) % 10;
					*(cell_value + 2) = 0;
				}
				else{
					strcpy(cell_value, "&nbsp;");
					*(cell_value + 6) = '0' + j + 1;
					*(cell_value + 7) = 0;
				}			
			}
			out<< " <td style=\"font-size:10pt";
			if(i == 6) out<< "; color:red";
			out<< "\">"<< cell_value<< "</td>";
		}
		out<< " </tr>"<< std::endl;
		++day;
	}
	out<< "</table></td> ";
	
	if(month > 0 && !((month + 1) % 6))
		out<< "</tr>"<< std::endl<< "<tr>";
		
	++month;
	day = next_month_day;
}

out<< "</tr> </table>"<< std::endl;
out<< "</head>"<< std::endl;
out<< "</html>"<< std::endl;
out.close();
return 0;
}