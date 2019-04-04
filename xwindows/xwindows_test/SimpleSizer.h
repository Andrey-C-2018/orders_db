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
	int margin_x;
	int max_widget_height;

	inline void init(const int margin_x) noexcept;
	inline void init(const int x_, const int y_, \
					const int width_, const int height_, \
					const int margin_x) noexcept;
public:
	CHorizontalSizer() noexcept;
	CHorizontalSizer(const int margin_x) noexcept;
	CHorizontalSizer(XWindow *parent_, \
			const int x_, const int y_, \
			const int width_, const int height_, \
			const int margin_x) noexcept;
	
	CHorizontalSizer(const CHorizontalSizer &obj) = default;
	CHorizontalSizer(CHorizontalSizer &&obj) = default;
	CHorizontalSizer &operator=(const CHorizontalSizer &obj) = default;
	CHorizontalSizer &operator=(CHorizontalSizer &&obj) = default;

	void addWidget(XWidget *widget, const int widget_width, const int widget_height);
	void addWidget(XWidget *widget, const Tchar *label, XSize size);
	void initNestedSizer(CVerticalSizer &sizer, const int row_height) const;
	void addNestedSizer(const CVerticalSizer &sizer);

	inline int getWidth() const;
	inline int getHeight() const;

	inline bool isNull() const;
	virtual ~CHorizontalSizer();
};

class CVerticalSizer {
	enum Defaults {
		MARGIN_X = 5, \
		MARGIN_Y = 5, \
		GAP_Y = 5
	};

	XWindow *parent;
	int x, y_initial, y;
	int width, height;
	int margin_x;
	int row_height;

	inline void init(const int margin_x) noexcept;
	inline void init(const int x_, const int y_, \
						const int width_, const int height_, \
						const int margin_x) noexcept;
public:
	CVerticalSizer() noexcept;
	CVerticalSizer(const int margin_x) noexcept;
	CVerticalSizer(XWindow *parent_, \
					const int x_, const int y_, \
					const int width_, const int height_, \
					const int row_height_, \
					const int margin_x) noexcept;

	CVerticalSizer(const CVerticalSizer &obj) = default;
	CVerticalSizer(CVerticalSizer &&obj) = default;
	CVerticalSizer &operator=(const CVerticalSizer &obj) = default;
	CVerticalSizer &operator=(CVerticalSizer &&obj) = default;

	void addWidget(XWidget *widget);
	void addWidget(XWidget *widget, const Tchar *label);
	void initNestedSizer(CHorizontalSizer &sizer) const;
	void addNestedSizer(const CHorizontalSizer &sizer);

	inline int getHeight() const;

	inline bool isNull() const;
	virtual ~CVerticalSizer();
};

//****************************************************************************

void CHorizontalSizer::init(const int margin_x) noexcept {

	init(margin_x, 0, 0, 0, margin_x);
}

void CHorizontalSizer::init(const int x_, const int y_, \
							const int width_, const int height_, \
							const int margin_x) noexcept {
	assert(width_ > 0);
	assert(height_ > 0);
	assert(2 * margin_x < width);

	x_initial = x_;
	x = x_initial + margin_x;
	y = y_;
	this->margin_x = margin_x;
	width = width_;
	height = height_;
	
	max_widget_height = 0;
}

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