#include <xwindows/XWidget.h>

class CHorizontalSizer {
	enum Defaults{
		MARGIN_X = 5, \
		GAP_X = 5
	};

	XWindow *parent;
	int x, y;
	const int width, height;
	int max_widget_height;

	CVerticalSizer nested_sizer;
public:
	CHorizontalSizer();
	CHorizontalSizer(XWindow *parent_, \
			const int x_, const int y_, \
			const int width_, const int height_);
	
	CHorizontalSizer(const CHorizontalSizer &obj) = default;
	CHorizontalSizer(CHorizontalSizer &&obj) = default;
	CHorizontalSizer &operator=(const CHorizontalSizer &obj) = default;
	CHorizontalSizer &operator=(CHorizontalSizer &&obj) = default;

	void add(XWidget *widget);
	void pushNestedSizer(CVerticalSizer sizer);
	inline int getHeight() const;
	virtual ~CHorizontalSizer();
};
