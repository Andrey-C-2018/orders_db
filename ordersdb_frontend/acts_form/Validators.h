#pragma once
#include <basic/XConv.h>
#include <xwindows/XWidget.h>

class CActNameValidator {
	enum { MAX_ACT_NAME_LEN = 25 };

	Tstring act_name;
	bool is_act_name_corrected;
	XWidget *act_name_holder;
public:
	CActNameValidator() : is_act_name_corrected(false), \
						act_name_holder(nullptr) { }

	CActNameValidator(XWidget *act_name_holder_) : \
						is_act_name_corrected(false), \
						act_name_holder(act_name_holder_) { }

	CActNameValidator(const CActNameValidator &obj) = default;
	CActNameValidator(CActNameValidator &&obj) = default;
	CActNameValidator &operator=(const CActNameValidator &obj) = default;
	CActNameValidator &operator=(CActNameValidator &&obj) = default;

	inline bool validate(ImmutableString<Tchar> act_str, \
								Tstring &error_str) {

		if (act_str.size > MAX_ACT_NAME_LEN) {
			error_str += _T("Ім'я акта не може бути довшим за ");
			Tchar buffer[3];
			error_str += XConv::ToString((size_t)MAX_ACT_NAME_LEN, buffer);
			error_str += _T(" симв.\n");
			return true;
		}
		act_name.clear();
		act_name.reserve(act_str.size);
		is_act_name_corrected = false;

		for (size_t i = 0; i < act_str.size; ++i) {
			Tchar ch = ToUpper(act_str.str[i]);
			is_act_name_corrected = is_act_name_corrected || \
									ch != act_str.str[i] || ch == _T(' ');

			if (ch == _T(' ')) continue;

			if (!((ch >= _T('0') && ch <= _T('9')) || \
				(ch >= _T('А') && ch <= _T('Я')) || \
				ch == _T('.') || ch == _T('І'))) {

				error_str += _T("Невірне ім'я акта: ");
				error_str += act_str.str;
				error_str += _T('\n');
				return true;
			}
			act_name += ch;
		}

		return true;
	}

	inline void refreshActNameWidget(bool &ignore_change_event) {
		
		assert(act_name_holder);
		if (is_act_name_corrected) {
			ignore_change_event = true;
			act_name_holder->SetLabel(act_name.c_str());
			ignore_change_event = false;
		}
	}

	inline const Tchar *getActName() const { return act_name.c_str(); }
};
