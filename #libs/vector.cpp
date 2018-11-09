template<class T> class CVector{
	T *data;
	size_t allocation_unit;
	size_t count;
	size_t arr_elements_count;
public:
	enum{ ALLOCATION_UNIT = 100 };
	CVector();
	CVector(size_t Allocation_unit);
	T operator[](size_t index);
	inline size_t size(){ return count; }
	void Add(T item);
	void Clear();
	//void Sort();
	~CVector();
};

//************************************************************

template<class T> CVector<T>::CVector(){

allocation_unit = ALLOCATION_UNIT;	
data = new T[allocation_unit];
count = 0;
arr_elements_count = allocation_unit;
}

template<class T> CVector<T>::CVector(size_t Allocation_unit){

allocation_unit = Allocation_unit;	
data = new T[allocation_unit];
count = 0;
arr_elements_count = allocation_unit;
}

template<class T> void CVector<T>::Add(T item){
	
if(count + 1 > arr_elements_count){
	T *temp;
	size_t i;
	
	arr_elements_count += allocation_unit;
	temp = new T[arr_elements_count];
	for(i = 0; i < arr_elements_count; i++)
		temp[i] = data[i];
	delete [] data;
	data = temp;
}
data[count] = item;
count++;
}

template<class T> void CVector<T>::Clear(){

count = 0;
}

template<class T> T CVector<T>::operator[](size_t index){
	
return data[index];
}

template<class T> CVector<T>::~CVector(){

delete [] data;
}
