#pragma once
#include "XWindow.h"
#include <basic/Exception.h>

class XFrame : public XWindow{
	class CRegisterPredicate {
		XFrame &frame;
		const Tchar *class_name;
	public:
		CRegisterPredicate(XFrame &frame_, \
							const Tchar *class_name_) noexcept : \
							frame(frame_), class_name(class_name_) { }

		inline bool operator()() {

			if (!_plRegisterWidget(class_name, frame.MainWndProc)) {
				XWindowException e(XWindowException::E_REGISTER_WINDOW, \
					_T("cannot register the frame window class:"));
				e << _T(" Class_name = ") << class_name;
				throw e;
			}
			return true;
		}
	};

	void OnCreate(XEvent *eve);
	void OnDestroy(XEvent *eve);
protected:
	int InitializeId() override { return SetIdToNull(); }
public:
	XFrame() noexcept;

	XFrame(XFrame &&obj) = default;
	XFrame(const XFrame &obj) = delete;
	XFrame &operator=(const XFrame &obj) = delete;
	XFrame &operator=(XFrame &&obj) = default;

	XFrame(const Tchar *class_name, const int flags, \
					 const Tchar *label, const int x, const int y,\
					 const int width, const int height);

	void Create(const Tchar *class_name, const int flags, \
					 const Tchar *label, const int x, const int y,\
					 const int width, const int height);
	virtual ~XFrame();
};