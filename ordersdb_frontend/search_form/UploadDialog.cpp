#include <xwindows/platform_specific.h>
#include <commctrl.h>
#include "UploadDialog.h"
#include "IUploadProgress.h"
#include "resource.h"

HWND hwnd_progress = nullptr;
UploadDialog *dialog = nullptr;
const CGrid *grid_to_use = nullptr;

//*****************************************************

class UploadProgress : public IUploadProgress {
	HWND hwnd_dialog;

public:
	UploadProgress(HWND hwnd_dialog_) : hwnd_dialog(hwnd_dialog_) { }

	void step() override {

		SendMessage(hwnd_progress, PBM_STEPIT, 0, 0);
	}

	void completed() override {

		SendMessage(hwnd_dialog, WM_COMMAND, ID_UPLOADCOMPLETED, 0);
	}

	virtual ~UploadProgress() { }
};

//*****************************************************

UploadDialog::UploadDialog(const size_t hidden_count_) : \
							hwnd_parent(nullptr), uploader(hidden_count_) {

	dialog = this;
}

void UploadDialog::init(HWND hwnd_parent_, std::shared_ptr<CDbTable> db_table_, \
						IProperties *props) {

	hwnd_parent = hwnd_parent_;
	uploader.init(db_table_, props);
}

void UploadDialog::upload(const CGrid *grid) {

	dialog->uploader.open();

	auto GlobalParams = CGlobalParams::GetInstance();
	HINSTANCE hinst = GlobalParams->GetHInstance();
	grid_to_use = grid;
	
	::DialogBox(hinst, MAKEINTRESOURCE(IDD_UPLOADDIALOG),
										hwnd_parent, (DLGPROC)DlgProc);
}

LRESULT CALLBACK UploadDialog::DlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {
		case WM_INITDIALOG:
			assert(dialog);
			assert(grid_to_use);

			hwnd_progress = GetDlgItem(hwnd, IDC_UPLOADPROGRESS);

			SendMessage(hwnd_progress, PBM_SETRANGE, 0, \
						MAKELPARAM(0, dialog->uploader.getRecordsCount()));
			SendMessage(hwnd_progress, PBM_SETSTEP, (WPARAM)1, 0);

			if (dialog->t.joinable()) dialog->t.join();
			dialog->t = std::thread([hwnd] {

				auto progress = std::make_shared<UploadProgress>(hwnd);
				dialog->uploader.upload(grid_to_use, progress);
				dialog->uploader.execExcel(progress);
			});
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wparam) == IDC_CANCELBUTTON)
				dialog->uploader.cancelUpload();

			if (wparam == ID_UPLOADCOMPLETED || LOWORD(wparam) == IDC_CANCELBUTTON) {
				
				::EndDialog(hwnd, wparam);

				auto except_in_thread = dialog->uploader.getErrorIfPresent();
				if (except_in_thread)
					ErrorBox(except_in_thread->what());
			}
			return TRUE;
	}
	return FALSE;
}

UploadDialog::~UploadDialog() {

	dialog->uploader.cancelUpload();
	if (t.joinable()) t.join();
}
