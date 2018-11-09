
unsigned lcs(const char *str1,
	 const char *str2,
	 const size_t len1,
	 const size_t len2){
size_t i, j, last_i, last_j;
int *D;
int operation, min;
unsigned count;

if(!len1) return len2;
if(!len2) return len1;

/*if(D_size < (len1 + 1)*(len2 + 1)){
	if(D) delete [] D;
	D = new int[(len1 + 1)*(len2 + 1)];
	D_size = (len1 + 1)*(len2 + 1);
}*/
D = new int[(len1 + 1)*(len2 + 1)];

for(i = 0; i < len1 + 1; i++)
	D[i] = i;

for(j = 1; j < len2 + 1; j++){
	D[(len1 + 1)*j] = j;
	for(i = 1; i < len1 + 1; i++){
		if(str1[i - 1] == str2[j - 1])
			D[i + (len1 + 1)*j] = D[i - 1 + (len1 + 1)*(j - 1)];
		else{
			if(D[i - 1 + (len1 + 1)*j] < D[i + (len1 + 1)*(j - 1)])
				min = D[i - 1 + (len1 + 1)*j];
			else
				min = D[i + (len1 + 1)*(j - 1)];

			if(min >= D[i - 1 + (len1 + 1)*(j - 1)])
				min = D[i - 1 + (len1 + 1)*(j - 1)];

			D[i + (len1 + 1)*j] = min + 1;
		}
	}
}

i = len1;
j = len2;
last_i = (size_t)-1;
last_j = (size_t)-1;
count = 0;
while(i >= 1 && j >= 1){
	if(str1[i - 1] == str2[j - 1] && i < last_i && j < last_j){
		last_i = i;
		last_j = j;
		count++;
	}
	if(D[i - 1 + (len1 + 1)*j] < D[i + (len1 + 1)*(j - 1)]){
		min = D[i - 1 + (len1 + 1)*j];
		operation = DELETE;
	} else{
		min = D[i + (len1 + 1)*(j - 1)];
		operation = INSERT;
	}

	if(min >= D[i - 1 + (len1 + 1)*(j - 1)]){
		min = D[i - 1 + (len1 + 1)*(j - 1)];
		operation = UPDATE;
	}
	switch(operation){
		case DELETE: i--; break;
		case INSERT: j--; break;
		case UPDATE: i--; j--;
	}
}
return count;
}

unsigned levenstein_distance(const char *str1,
			   const char *str2,
			   const size_t len1,
			   const size_t len2){
unsigned *D, min;
size_t i, j;

if(!len1) return len2;
if(!len2) return len1;

D = new unsigned[(len2 + 1)*2];
for(i = 0; i < len2 + 1; i++)
	D[2*i] = i;

for(j = 1; j < (len1 + 1); j++){
	D[1] = j;
	for(i = 1; i < (len2 + 1); i++){
		if(str1[j - 1] == str2[i - 1])
			D[2*i + 1] = D[2*(i - 1)];
		else{
			min = (D[2*i] < D[2*i - 1]) ? D[2*i] : D[2*i - 1];
			min = (min >= D[2*(i - 1)]) ? D[2*(i - 1)] : min;
			D[2*i + 1] = min + 1;
		}
		D[2*(i - 1)] = D[2*i - 1];
	}
	D[2*len2] = D[2*len2 + 1];
}

min = D[2*len2 + 1];
delete [] D;

return min;
}
