#include <xwindows/XWidget.h>

class CVerticalSizer {
	enum Defaults{
		MARGIN_Y = 5, \
		GAP_Y = 5
	};

	XWindow *parent;
	int x, y;
	const int width, height;
	const int row_height;

	CHorizontalSizer nested_sizer;
public:
	CVerticalSizer();
	CVerticalSizer(XWindow *parent_, \
			const int x_, const int y_, \
			const int width_, const int height_, \
			const int row_height_);
	
	CVerticalSizer(const CVerticalSizer &obj) = default;
	CVerticalSizer(CVerticalSizer &&obj) = default;
	CVerticalSizer &operator=(const CVerticalSizer &obj) = default;
	CVerticalSizer &operator=(CVerticalSizer &&obj) = default;

	void add(XWidget *widget);
	void pushNestedSizer(CHorizontalSizer sizer);
	void popNestedSizer();

	inline CHorizontalSizer &getNestedSizer() const;
	inline int getHeight() const;

	virtual ~CVerticalSizer();
};

//****************************************************************************

CHorizontalSizer &CVerticalSizer::getNestedSizer() const {

	return nested_sizer;
}

int CVerticalSizer::getHeight() const {

	return y;
}