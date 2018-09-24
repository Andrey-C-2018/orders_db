#pragma once

template <typename NumericType_> \
struct CNumericTypeWrapper {
	NumericType_ num;
	inline CNumericTypeWrapper() noexcept : num(0) { }
	inline CNumericTypeWrapper(const CNumericTypeWrapper &obj) = default;
	inline CNumericTypeWrapper(const NumericType_ num_) : num(num_) { }
	inline CNumericTypeWrapper &operator=(const CNumericTypeWrapper &obj) = default;
	inline CNumericTypeWrapper &operator=(const NumericType_ num_) { num = num_; return *this; }
	inline bool operator<(const CNumericTypeWrapper &obj) const { return num < obj.num; }
	inline bool operator>(const CNumericTypeWrapper &obj) const { return num > obj.num; }
	inline bool operator==(const CNumericTypeWrapper &obj) const { return num == obj.num; }
	inline bool operator<(const NumericType_ num_) const { return num < num_; }
	inline bool operator>(const NumericType_ num_) const { return num > num_; }
	inline bool operator==(const NumericType_ num_) const { return num == num_; }
};

template <typename NumericType_> \
inline bool operator<(const NumericType_ l, const CNumericTypeWrapper<NumericType_> r) { 
	return l < r.num; 
}

template <typename NumericType_> \
inline bool operator>(const NumericType_ l, const CNumericTypeWrapper<NumericType_> r) {
	return l > r.num;
}

template <typename NumericType_> \
inline bool operator==(const NumericType_ l, const CNumericTypeWrapper<NumericType_> r) {
	return l == r.num;
}