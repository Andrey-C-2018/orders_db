#pragma once
#include <vector>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <codecvt>
#include <basic/Exception.h>
#include <basic/chars.h>
#include <basic/XConv.h>
#include "ITable.h"

template <typename Tstring_> \
class CCsvRecord : public IRecord<typename Tstring_::value_type>{
	typedef typename Tstring_::value_type Tchar_;
	typedef typename Tstring_::const_iterator StrConstIterator;

	static Tstring_ empty_str;
	Tstring_ &rec;
	size_t col_count;
	mutable size_t curr_col;
	mutable StrConstIterator p_begin, p_end;
	mutable Tstring_ curr_col_value;
	Tchar_ delimiter;
	const Tchar_ *null_str;
public:
	CCsvRecord(const Tchar_ delimiter_);
	CCsvRecord(Tstring_ &rec_, const size_t col_count_, \
				const Tchar_ delimiter_);
	void init(Tstring_ &rec_, const size_t col_count_);
	
	CCsvRecord(const CCsvRecord &obj) = default;
	CCsvRecord(CCsvRecord &&obj) = default;
	CCsvRecord &operator=(const CCsvRecord &obj) = default;
	CCsvRecord &operator=(CCsvRecord &&obj) = default;
		
	static size_t evalColumnsCount(const Tstring_& rec, const Tchar_ delimiter_);
	size_t size() const override;
	const Tchar_ *getColValueAsCharArray(const size_t index) const override;

	void setNullStr(const Tchar_ *null_str) { this->null_str = null_str; }
		
	virtual ~CCsvRecord();
};

class CCsvTableException : public XException{
public:
	enum Errors{
		E_FILE_NOT_FOUND = 1, \
		E_FILE_IS_EMPTY = 2
	};
	
	CCsvTableException(const int err_code, const Tchar *err_descr);
	CCsvTableException(const CCsvTableException &obj);
	CCsvTableException(CCsvTableException &&obj) = default;
	virtual ~CCsvTableException();
};

template <typename Tstring_> \
class CCsvTable : public ITable<Tstring_> {
	typedef typename Tstring_::value_type Tchar_;
	typedef typename Tstring_::const_iterator StrConstIterator;
	typedef typename std::basic_ifstream<Tchar_> Tifstream;
	typedef typename std::istreambuf_iterator<Tchar_> istr_iter;

	mutable Tifstream in;
	std::vector<Tstring_> col_names;
	mutable Tstring_ buffer;
	mutable std::shared_ptr<CCsvRecord<Tstring_> > rec;
	std::vector<size_t> rec_offsets;
	size_t file_size;
	Tchar_ crlf;
	size_t crlf_size;
	Tchar_ delimiter;

	std::shared_ptr<IRecord<Tchar_> > getRecordInternal(const size_t index) const;
	
	enum Defaults{
		DEF_REC_OFFSETS_SIZE = 50
	};

	inline void initFileStream(std::ifstream &f, const char *encoding){ }
	inline void initFileStream(std::wifstream &f, const char *encoding);
public:
	CCsvTable(const char *file_name, const char *encoding, \
				const Tchar_ delimiter_, const bool read_header);
				
	CCsvTable(const CCsvTable &obj) = delete;
	CCsvTable(CCsvTable &&obj) = default;
	CCsvTable &operator=(const CCsvTable &obj) = delete;
	CCsvTable &operator=(CCsvTable &&obj) = default;
				
	size_t getColumnsCount() const override;
	size_t getRecordsCount() const override;
	std::shared_ptr<const IRecord<Tchar_> > getRecord(const size_t index) const override;
	std::shared_ptr<IRecord<Tchar_> > getRecord(const size_t index) override;
	size_t getColIndexByName(const Tchar_ *col_name) const override;
	const Tchar_ *getColName(const size_t index) const override;
	virtual ~CCsvTable();
};

//**************************************************************

template <typename Tstring_, typename Predicate> \
inline void traverse(typename Tstring_::const_iterator p_start, \
						typename Tstring_::const_iterator p_end, \
						const typename Tstring_::value_type delimiter, \
						Predicate &&pred) {
	static_assert(std::is_same<Tstring, std::string>::value || \
					std::is_same<Tstring, std::wstring>::value, \
					"traverse() accepts only string/wstring as its template parameter");

	typename Tstring_::const_iterator p_curr = p_start;
	bool go_on = true, delimiter_found = false;

	typedef typename Tstring_::value_type Tchar_;

	while (go_on && p_curr != p_end) {
		auto p_prev = p_curr;
		size_t ekrans_count = 0;
		bool ekran = (*p_curr == getQuoteChar('"'));

		p_curr = std::find_if(p_curr, p_end, \
			[&ekrans_count, ekran, &delimiter_found, delimiter](const Tchar_ ch) -> bool {
			ekrans_count += (ekran && ch == getQuoteChar('"'));
			delimiter_found = ch == delimiter && (ekrans_count % 2) == 0;
			return delimiter_found;
		});
		go_on = pred(p_prev, p_curr, ekran && (ekrans_count % 2) == 0);
		if (p_curr != p_end) ++p_curr;
	}

	if (go_on && delimiter_found && p_curr == p_end)
		pred(p_end, p_end, false);
};

//**************************************************************

template <typename Tstring_> \
class CCsvRecordPositionPred {
	typedef typename Tstring_::value_type Tchar_;
	typedef typename Tstring_::const_iterator StrConstIterator;

	bool is_ekranned;
	StrConstIterator p_begin, p_end;
	const size_t curr_col, col_to_find;
	size_t calls_count;
public:
	CCsvRecordPositionPred(const size_t curr_col_,
							const size_t col_to_find_) noexcept : \
							is_ekranned(false), curr_col(curr_col_), \
							col_to_find(col_to_find_), calls_count(0) { }

	CCsvRecordPositionPred(const CCsvRecordPositionPred &obj) = default;
	CCsvRecordPositionPred(CCsvRecordPositionPred &&obj) = default;
	CCsvRecordPositionPred &operator=(const CCsvRecordPositionPred &obj) = default;
	CCsvRecordPositionPred &operator=(CCsvRecordPositionPred &&obj) = default;

	inline bool operator()(StrConstIterator p_begin, \
							StrConstIterator p_end, \
							const bool is_ekranned) noexcept {

		this->p_begin = p_begin;
		this->p_end = p_end;
		this->is_ekranned = is_ekranned;

		bool column_not_found = (curr_col + calls_count) != col_to_find;
		++calls_count;
		return column_not_found;
	}

	inline void removeEkrans() {

		if (is_ekranned && std::distance(p_begin, p_end) >= 3) {
			while (p_begin != p_end - 1 && \
					*p_begin == getQuoteChar('"') && \
					*(p_end - 1) == getQuoteChar('"')) {

				++p_begin;
				--p_end;
			}
		}
	}

	inline void assignFoundValueToStr(Tstring_ &str) {

		removeEkrans();
		str.assign(p_begin, p_end);
	}

	inline void getPositionIters(StrConstIterator &p_begin, \
									StrConstIterator &p_end) const noexcept {

		p_begin = this->p_begin;
		p_end = this->p_end;
	}

	inline size_t getCurrentColumn() const noexcept {

		return calls_count ? curr_col + calls_count - 1 : curr_col;
	}

	inline bool isValueEkranned() const noexcept { return is_ekranned; }
};

//**************************************************************

template <typename Tstring_> Tstring_ CCsvRecord<Tstring_>::empty_str;

template <typename Tstring_> \
CCsvRecord<Tstring_>::CCsvRecord(const Tchar_ delimiter_) : \
									rec(empty_str), \
									col_count(0), curr_col(0), \
									p_begin(rec.cend()), \
									p_end(rec.cend()), \
									delimiter(delimiter_), \
									null_str(getEmptyStr(Tchar_())) { }

template <typename Tstring_> \
CCsvRecord<Tstring_>::CCsvRecord(Tstring_ &rec_, const size_t col_count_, \
									const Tchar_ delimiter_) : \
									rec(rec_), \
									delimiter(delimiter_), 
									null_str(getEmptyStr(Tchar_())) {

	init(rec_, col_count_);
}

template <typename Tstring_> \
void CCsvRecord<Tstring_>::init(Tstring_ &rec_, const size_t col_count_) {

	rec = rec_;
	col_count = col_count_;

	CCsvRecordPositionPred<Tstring_> csv_pred(0, 0);
	traverse<Tstring_>(rec.cbegin(), rec.cend(), delimiter, csv_pred);

	csv_pred.getPositionIters(p_begin, p_end);
	curr_col = csv_pred.getCurrentColumn();

	csv_pred.assignFoundValueToStr(curr_col_value);
}

template <typename Tstring_> \
size_t CCsvRecord<Tstring_>::evalColumnsCount(const Tstring_& rec, const Tchar_ delimiter_) {

	size_t count = 0;
	traverse<Tstring_>(rec.cbegin(), rec.cend(), delimiter_, \
		[&count](StrConstIterator p_begin, \
			StrConstIterator p_end, \
			const bool is_ekranned) -> bool {
		++count;
		return true;
	});

	return count;
}

template <typename Tstring_> \
size_t CCsvRecord<Tstring_>::size() const {

	return col_count;
}

template <typename Tstring_> \
const typename Tstring_::value_type* \
	CCsvRecord<Tstring_>::getColValueAsCharArray(const size_t index) const {

	assert(index < col_count);
	if (index != curr_col) {
		if (index < curr_col) {
			p_begin = rec.cbegin();
			p_end = rec.cend();
			curr_col = 0;
		}
		else {
			if (p_end == rec.cend()) return null_str;

			p_begin = p_end + 1;
			++curr_col;
		}

		CCsvRecordPositionPred<Tstring_> csv_pred(curr_col, index);
		traverse<Tstring_>(p_begin, rec.cend(), delimiter, csv_pred);

		csv_pred.getPositionIters(p_begin, p_end);
		curr_col = csv_pred.getCurrentColumn();

		csv_pred.assignFoundValueToStr(curr_col_value);
	}
	return curr_col_value.c_str();
}

template <typename Tstring_> \
CCsvRecord<Tstring_>::~CCsvRecord() { }

//**************************************************************

enum CrLf {
	CR = 0x0d, \
	LF = 0x0a
};

template <typename Tstring_> \
void CCsvTable<Tstring_>::initFileStream(std::wifstream &f, const char *encoding) {

	f.unsetf(std::ios::skipws);
	std::locale loc(encoding);
	f.imbue(loc);
}

template <typename Tstring_> \
CCsvTable<Tstring_>::CCsvTable(const char *file_name, const char *encoding, \
								const Tchar_ delimiter_, const bool read_header) : \
								file_size(0), crlf(0), crlf_size(0), delimiter(delimiter_) {
	assert(file_name);
	assert(encoding);

	in.open(file_name, std::ios::in | std::ios::binary);
	if (!in.is_open()) {
		CCsvTableException e(CCsvTableException::E_FILE_NOT_FOUND, _T("file not found: "));
		e << file_name;
		throw e;
	}

	initFileStream(in, encoding);

	rec_offsets.reserve(DEF_REC_OFFSETS_SIZE);
	size_t crlf_count = 0;
	Tstring_ &buffer_ref = buffer;
	auto p = std::find_if(istr_iter(in), istr_iter(), \
		[&buffer_ref](const Tchar_ ch) {
		buffer_ref += ch;
		return ch == CR || ch == LF;
	});
	if (p != istr_iter()) {
		buffer.erase(buffer.end() - 1);
		crlf = *p;
		crlf_size = 1;
		++p;
		if (p != istr_iter() && *p == LF) {
			++crlf_size;
			++p;
		}

		rec_offsets.push_back((size_t)in.tellg());
		crlf_count = 1;
	}

	size_t col_count = CCsvRecord<Tstring_>::evalColumnsCount(buffer, delimiter);
	if (!col_count) {
		in.close();
		throw CCsvTableException(CCsvTableException::E_FILE_IS_EMPTY, _T("the file is empty"));
	}

	rec = std::make_shared<CCsvRecord<Tstring_> >(delimiter);
	if (read_header) {
		rec->init(buffer, col_count);
		for (size_t i = 0; i < col_count; ++i)
			col_names.push_back(rec->getColValueAsCharArray(i));
	}
	else {
		Tchar_ buffer[30];
		for (size_t i = 0; i < col_count; ++i)
			col_names.push_back(XConv::ToString(i, buffer));
		
		rec_offsets.insert(rec_offsets.begin(), 0);
	}

	while (p != istr_iter()) {
		if (*p == crlf) {
			std::advance(p, crlf_size);

			rec_offsets.push_back((size_t)in.tellg());
			++crlf_count;
		}
		else {
			++p;
			crlf_count = 0;
		}
	}

	file_size = (size_t)in.tellg();

	rec_offsets.erase(rec_offsets.end() - crlf_count, rec_offsets.end());
	file_size -= crlf_count * crlf_size;
}

template <typename Tstring_> \
size_t CCsvTable<Tstring_>::getColumnsCount() const {

	return col_names.size();
}

template <typename Tstring_> \
size_t CCsvTable<Tstring_>::getRecordsCount() const {

	return rec_offsets.size();
}

template <typename Tstring_> \
std::shared_ptr<IRecord<typename Tstring_::value_type> > \
	CCsvTable<Tstring_>::getRecordInternal(const size_t index) const {

	assert(index < rec_offsets.size());
	assert(rec.get());
	size_t count = index + 1 < rec_offsets.size() ? \
		rec_offsets[index + 1] - rec_offsets[index] - crlf_size : \
		file_size - rec_offsets[index];

	buffer.clear();
	in.seekg(rec_offsets[index]);
	std::copy_n(istr_iter(in), count, std::back_inserter(buffer));
	rec->init(buffer, col_names.size());
	return rec;
}

template <typename Tstring_> \
std::shared_ptr<const IRecord<typename Tstring_::value_type> > \
	CCsvTable<Tstring_>::getRecord(const size_t index) const {

	return getRecordInternal(index);
}

template <typename Tstring_> \
std::shared_ptr<IRecord<typename Tstring_::value_type> > \
	CCsvTable<Tstring_>::getRecord(const size_t index) {

	return getRecordInternal(index);
}

template <typename Tstring_> \
size_t CCsvTable<Tstring_>::getColIndexByName(const Tchar_ *col_name) const {

	assert(col_name);
	auto p = std::find(col_names.cbegin(), col_names.cend(), col_name);

	return p != col_names.cend() ? \
		std::distance(col_names.cbegin(), p) : ITable<Tstring_>::NOT_FOUND;
}

template <typename Tstring_> \
const typename Tstring_::value_type* \
	CCsvTable<Tstring_>::getColName(const size_t index) const {

	assert(index < col_names.size());
	return col_names[index].c_str();
}

template <typename Tstring_> \
CCsvTable<Tstring_>::~CCsvTable() {

	if (in.is_open()) in.close();
}
