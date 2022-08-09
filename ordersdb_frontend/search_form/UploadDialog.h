#pragma once
#include <thread>
#include <Windows.h>
#include "Uploader.h"

class UploadDialog {
	HWND hwnd_parent;
	Uploader uploader;
	std::thread t;

	UploadDialog(const UploadDialog &obj) = delete;
	UploadDialog(UploadDialog &&obj) = delete;
	UploadDialog &operator=(const UploadDialog &obj) = delete;
	UploadDialog &operator=(UploadDialog &&obj) = delete;

	static LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, \
									WPARAM wparam, LPARAM lparam);

public:
	UploadDialog(const size_t hidden_count_);

	void init(HWND hwnd_parent_, \
				std::shared_ptr<CDbTable> db_table_, \
				IProperties *props);
	void upload(const CGrid *grid);

	virtual ~UploadDialog();
};

