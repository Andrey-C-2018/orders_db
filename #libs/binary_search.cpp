#include <stdio.h>

int *binary_search(int *first, int *last, int item){
int *mid;

if(first == last || !first || !last) return NULL;
while(first <= last){
	mid = first + (last - first)/2;
	if(*mid == item) return mid;

	if(*mid < item) first = mid + 1;
	else last = mid - 1;
}

return NULL;
}

int main(){
int a[] = {0, 0, 1, 2, 12, 14, 45, 230, 2467};
int b[] = {0, 1};
int c[] = {1};
int *p;		 

p = binary_search(a, &a[0] + sizeof(a)/sizeof(int), 12);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(a, &a[0] + sizeof(a)/sizeof(int), 0);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(a, &a[0] + sizeof(a)/sizeof(int), 1);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(a, &a[0] + sizeof(a)/sizeof(int), 2467);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(a, &a[0] + sizeof(a)/sizeof(int), 267);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(a, &a[0] + sizeof(a)/sizeof(int), 26007);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(a, &a[0] + sizeof(a)/sizeof(int), -1);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(b, &b[0] + sizeof(b)/sizeof(int), 0);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(b, &b[0] + sizeof(b)/sizeof(int), 1);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(b, &b[0] + sizeof(b)/sizeof(int), -1);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(b, &b[0] + sizeof(b)/sizeof(int), 10);
if(p) printf("%d\n", *p);
else puts("Not found");

puts("");

p = binary_search(c, &c[0] + sizeof(c)/sizeof(int), 0);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(c, &c[0] + sizeof(c)/sizeof(int), 1);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(c, &c[0] + sizeof(c)/sizeof(int), -1);
if(p) printf("%d\n", *p);
else puts("Not found");

p = binary_search(c, &c[0] + sizeof(c)/sizeof(int), 100);
if(p) printf("%d\n", *p);
else puts("Not found");
return 0;
}