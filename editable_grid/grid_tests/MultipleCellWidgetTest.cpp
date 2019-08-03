#include <basic/tstream.h>
#include <UnitTest++/UnitTest++.h>
#include <editable_grid/MultipleCellWidget.h>

class CNullWidget : public IGridCellWidget {
	static size_t ref_counter;
public:
	CNullWidget() { ++ref_counter; }
	static inline size_t GetObjectsCount() { return ref_counter; }

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override { }

	void SetOnChangeHandler(XEventHandlerData on_change) override { }
	void SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> handler) override { }
	void SetOnGetFocusHandler(XEventHandlerData on_get_focus) override { }
	void SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) override { }
	void SetOnKeyPressHandler(XEventHandlerData on_key_press) override { }

	void SetCurrentField(const size_t field) override { }

	void Show() override { }
	void Hide() override { }
	void MoveWindow(const int x, const int y, \
					const int width, const int height) override { }
	void SetFocus() override { }
	bool HasFocus() const override { return false; }

	ImmutableString<Tchar> GetLabel() override { return ImmutableString<Tchar>(nullptr, 0); }
	void SetLabel(ImmutableString<Tchar> label) override { }

	int GetId() const override { return 0; }

	bool Validate() const override { return true; }
	bool Validate(ImmutableString<Tchar> label) const override { return true; }

	virtual ~CNullWidget() { --ref_counter; }
};

size_t CNullWidget::ref_counter = 0;

SUITE(MultipleCellWidget_tests) {

	TEST(SetDefWidget) {

		auto null_widget = new CNullWidget();
		auto multi = new CMultipleCellWidget();
		multi->SetDefaultWidget(null_widget);

		auto curr_widget = multi->GetCurrentWidget();
		CHECK_EQUAL(curr_widget, null_widget);

		delete multi;
		CHECK_EQUAL(0, CNullWidget::GetObjectsCount());
	}

	TEST(AddWidget) {

		auto multi = new CMultipleCellWidget();

		auto null_widget1 = new CNullWidget();
		multi->AddCellWidget(2, null_widget1);
		CHECK_EQUAL(1, multi->GetWidgetsCount());
		CHECK_EQUAL(1, multi->GetAssignmentsCount());

		auto null_widget2 = new CNullWidget();
		multi->AddCellWidget(0, null_widget2);
		CHECK_EQUAL(2, multi->GetWidgetsCount());
		CHECK_EQUAL(2, multi->GetAssignmentsCount());

		multi->AddCellWidget(1, null_widget1);
		CHECK_EQUAL(2, multi->GetWidgetsCount());
		CHECK_EQUAL(3, multi->GetAssignmentsCount());

		multi->AddCellWidget(1, null_widget2);
		CHECK_EQUAL(2, multi->GetWidgetsCount());
		CHECK_EQUAL(3, multi->GetAssignmentsCount());

		multi->AddCellWidget(2, null_widget2);
		CHECK_EQUAL(1, multi->GetWidgetsCount());
		CHECK_EQUAL(3, multi->GetAssignmentsCount());

		multi->AddCellWidget(0, new CNullWidget());
		CHECK_EQUAL(2, multi->GetWidgetsCount());
		CHECK_EQUAL(3, multi->GetAssignmentsCount());

		delete multi;
		CHECK_EQUAL(0, CNullWidget::GetObjectsCount());
	}


}