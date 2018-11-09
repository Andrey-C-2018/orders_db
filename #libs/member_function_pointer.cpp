#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;

#define interface struct

interface CIndexedVector{
	typedef size_t IndexType;
	typedef vector<IndexType>::iterator IndexIterator;
	typedef bool (CIndexedVector::*CompFuncPointer)(IndexIterator l, IndexIterator r);

	virtual void sort(IndexIterator first,\
					IndexIterator last,\
					CompFuncPointer Compare) = 0;
	virtual bool CompFunction(IndexIterator l, IndexIterator r) = 0;
	CIndexedVector(){ }
	virtual ~CIndexedVector(){ }
};

class CMetaTable : private CIndexedVector{
	struct CFieldRecord{
		int id;
		string name;
		int f_type;
		int id_table;
		bool operator==(const CFieldRecord &obj){
		
			return this->id == obj.id;
		}

		bool operator<(const CFieldRecord &obj){
		
			return this->id < obj.id;
		}

	};

	vector<CFieldRecord> fields;
	vector<size_t> indexes;

	void sort(IndexIterator first,\
					IndexIterator last,\
					CompFuncPointer Compare){
		std::iterator_traits< IndexIterator >::difference_type d;
		IndexIterator i, j;

		for(d = 1; d <= ( last - first ) / 9; d = 3*d + 1) ;
		for(; d > 0; d /= 3)
			for(i = first + d; i != last; ++i){
				for(j = i; j - first >= d && (this->*Compare)(j, ( j - d )); j -= d)
					std::swap( *j, *( j - d ) );
			}
	}

	bool CompFunction(IndexIterator l, IndexIterator r){

		return fields[*l].name.compare(fields[*r].name) < 0;
	}

public:
	CMetaTable();
	void Add();
	void Sort();
	void Print();
	virtual ~CMetaTable(){}
};

//*************************************************************

CMetaTable::CMetaTable(){

fields.clear();
indexes.clear();
}

void CMetaTable::Add(){
vector<CFieldRecord>::iterator p_field;
vector<size_t>::iterator p_index;
time_t t;
CFieldRecord rec;
size_t i;
char buf[15];

srand((unsigned)time(&t));
fields.reserve(100);
indexes.reserve(100);
for(i = 0; i < 100; i++){
	rec.id = 100*rand() / RAND_MAX;
	rec.name = itoa(rec.id, buf, 10);
	fields.push_back(rec);
	indexes.push_back(i);
}
}

void CMetaTable::Sort(){

sort(indexes.begin(), indexes.end(), &CIndexedVector::CompFunction);
}

void CMetaTable::Print(){
vector<CFieldRecord>::iterator p_field;
vector<size_t>::iterator p_index;

cout<< "Fields: "<< endl;
for(p_field = fields.begin(); p_field != fields.end(); p_field++)
	cout<< (*p_field).id << " ";

cout<< endl;
cout<< "Fields: "<< endl;
for(p_index = indexes.begin(); p_index != indexes.end(); p_index++)
	cout<< fields[*p_index].id << " ";
}

int main(){
CMetaTable m;

m.Add();
m.Sort();
m.Print();
cin.get();
return 0;
}
