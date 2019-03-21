
class CVerticalSizer {
	const int width;
	const int row_height;

public:
	CVerticalSizer(const int width_, const int row_height_);
	void add(XWidget *widget);
	virtual ~CVerticalSizer();
};

CVerticalSizer::CVerticalSizer(XWindow *parent, \
				const int x_, const int y_, \
				const int width_, const int height_, \
				const int row_height_) : \
				width(width_), row_height(row_height_) {

	
}

void CVerticalSizer::add(XWidget *widget) {
	
	assert(widget);
	widget->Create();
}

~CVerticalSizer();
