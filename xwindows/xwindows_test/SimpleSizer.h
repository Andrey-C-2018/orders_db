#include <xwindows/XWidget.h>

class CVerticalSizer;

class CHorizontalSizer {
	enum Defaults{
		MARGIN_X = 5, \
		GAP_X = 5
	};

	XWindow *parent;
	int x_initial, x, y;
	int width, height;
	int max_widget_height;

public:
	CHorizontalSizer() noexcept;
	CHorizontalSizer(XWindow *parent_, \
			const int x_, const int y_, \
			const int width_, const int height_) noexcept;
	
	CHorizontalSizer(const CHorizontalSizer &obj) = default;
	CHorizontalSizer(CHorizontalSizer &&obj) = default;
	CHorizontalSizer &operator=(const CHorizontalSizer &obj) = default;
	CHorizontalSizer &operator=(CHorizontalSizer &&obj) = default;

	void addWidget(XWidget *widget, const int widget_width, const int widget_height);
	void initNestedSizer(CVerticalSizer &sizer, const int row_height) const;
	void addNestedSizer(const CVerticalSizer &sizer);

	inline int getWidth() const;
	inline int getHeight() const;

	inline bool isNull() const;
	virtual ~CHorizontalSizer();
};

class CVerticalSizer {
	enum Defaults {
		MARGIN_Y = 5, \
		GAP_Y = 5
	};

	XWindow *parent;
	int x, y_initial, y;
	int width, height;
	int row_height;

public:
	CVerticalSizer() noexcept;
	CVerticalSizer(XWindow *parent_, \
					const int x_, const int y_, \
					const int width_, const int height_, \
					const int row_height_) noexcept;

	CVerticalSizer(const CVerticalSizer &obj) = default;
	CVerticalSizer(CVerticalSizer &&obj) = default;
	CVerticalSizer &operator=(const CVerticalSizer &obj) = default;
	CVerticalSizer &operator=(CVerticalSizer &&obj) = default;

	void addWidget(XWidget *widget);
	void initNestedSizer(CHorizontalSizer &sizer) const;
	void addNestedSizer(const CHorizontalSizer &sizer);

	inline int getHeight() const;

	inline bool isNull() const;
	virtual ~CVerticalSizer();
};

//****************************************************************************

int CHorizontalSizer::getWidth() const {

	return width;
}

int CHorizontalSizer::getHeight() const {

	return max_widget_height;
}

bool CHorizontalSizer::isNull() const {

	return !(parent && x > x_initial && width && height && max_widget_height);
}

//****************************************************************************

int CVerticalSizer::getHeight() const {

	return y;
}

bool CVerticalSizer::isNull() const {

	return !(parent && y > y_initial && width && height && row_height);
}