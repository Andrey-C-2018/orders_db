#pragma once
#include <memory>
#include <fstream>
#include <basic/Exception.h>
#include <basic/XString.h>

class CUploadException : public XException {
public:
	enum {
		E_FILE_OPEN = 1,
		E_CONFIG = 2
	};

	CUploadException(const int err_code, const Tchar *err_descr);
	CUploadException(const CUploadException &obj);
	CUploadException(CUploadException &&obj) = default;
	virtual ~CUploadException();
};

struct IProperties;
struct IUploadProgress;
class CDbTable;
class CGrid;

class Uploader {
	std::ofstream out;
	std::shared_ptr<CDbTable> db_table;
	const size_t hidden_count;
	XString<char> path, excel_path;

	volatile bool cancel_upload;
	std::shared_ptr<XException> except_in_thread;
	
	inline void addHeader();
public:
	Uploader(const size_t hidden_count_);

	Uploader(const Uploader &obj) = delete;
	Uploader(Uploader &&obj) = default;
	Uploader &operator=(const Uploader &obj) = delete;
	Uploader &operator=(Uploader &&obj) = default;

	void init(std::shared_ptr<CDbTable> db_table_, \
				IProperties *props);
	void open();
	size_t getRecordsCount() const;

	void upload(const CGrid *grid, std::shared_ptr<IUploadProgress> progress) noexcept;
	void execExcel(std::shared_ptr<IUploadProgress> progress) noexcept;
	inline void cancelUpload();
	std::shared_ptr<const XException> getErrorIfPresent() const noexcept;

	virtual ~Uploader();
};

//*****************************************************

void Uploader::addHeader() {

	assert(out.is_open());
	out << "<!DOCTYPE HTML PUBLIC \" -//W3C//DTD HTML 4.0 Transitional//EN\">" << std::endl;
	out << "<html>" << std::endl;
	out << " <head>" << std::endl;
	out << "  <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>" << std::endl;
	out << "  <title></title>" << std::endl;
	out << " </head>" << std::endl;
	out << " <body>" << std::endl;
	out << "  <table border=1>" << std::endl;
}

void Uploader::cancelUpload() {

	cancel_upload = true;
}