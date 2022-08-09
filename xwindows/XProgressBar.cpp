#include "XProgressBar.h"
#include <commctrl.h>

XProgressBar::XProgressBar() {

	CheckAndInitWindowClassName(PROGRESS_CLASS);
}

XProgressBar::XProgressBar(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) {

	CheckAndInitWindowClassName(PROGRESS_CLASS);
	Create(parent, flags, label, x, y, width, height);
}

void XProgressBar::SetParams(int range, int step) {

	assert(range >= 0);
	assert(step >= 0);

	::SendMessage(this->GetInternalHandle(), PBM_SETRANGE, 0, MAKELPARAM(0, range));
	::SendMessage(this->GetInternalHandle(), PBM_SETSTEP, (WPARAM)step, 0);
}

XProgressBar::~XProgressBar() { }
