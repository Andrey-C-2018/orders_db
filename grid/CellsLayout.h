#pragma once
#include <memory>
#include "FieldComposer.h"
#include "ISharedComposer.h"
#include "GridLine.h"
#include "IGridCellsContainer.h"

template <typename Data, typename Items> \
class TCellsLayout : public IGridCellsContainer {
protected:
	Data data;
	Items items;

	TCellsLayout(const TCellsLayout &obj) = delete;
	TCellsLayout &operator=(const TCellsLayout &obj) = delete;
	TCellsLayout &operator=(TCellsLayout &&obj) = delete;
public:
	TCellsLayout() { }

	TCellsLayout(Data data_) : data(data_) { }

	TCellsLayout(Data data_, Items &&items_) : \
					data(data_), \
					items(std::move(items_)){ }

	TCellsLayout(TCellsLayout &&obj) : \
					data(std::move(obj.data)), \
					items(std::move(obj.items)){ }

	void Init(Data data) { 
		
		this->data = data;
	}

	size_t count() const override { return data.size(); }

	void EndScroll(const int scroll_pos) override {

		items.EndScroll(scroll_pos);
	}

	void Reload() override {

		items.Reload();
	}

	virtual ~TCellsLayout(){ }
};

template <typename Data, typename Cells> \
class TSimpleFieldLayout : \
		public TCellsLayout<Data, TCellWithLine<Cells, CHorizontalLine> > {
	CFieldComposer composer;
	int upper_bound;
	CVerticalLine vertical_ending_line;

	TSimpleFieldLayout(const TSimpleFieldLayout &obj) = delete;
	TSimpleFieldLayout &operator=(const TSimpleFieldLayout &obj) = delete;
	TSimpleFieldLayout &operator=(TSimpleFieldLayout &&obj) = delete;
public:
	TSimpleFieldLayout() : composer(this), upper_bound(0) { }

	TSimpleFieldLayout(Data data_) : TCellsLayout(data_), \
								composer(this), \
								upper_bound(0) { }

	TSimpleFieldLayout(TSimpleFieldLayout &&obj) : \
					TCellsLayout(std::move(obj)), \
					composer(this, std::move(obj.composer)), \
					upper_bound(obj.upper_bound), \
			vertical_ending_line(std::move(obj.vertical_ending_line)) {
	
		obj.upper_bound = 0;
	}

	IDrawable &GetByIndex(const size_t index) override {
		std::pair<size_t, size_t> field_and_record = data.GetCellCoords(index);

		items.Init(data[index], field_and_record.first, field_and_record.second);
		return items;
	}

	void SetBounds(const int left_bound, const int upper_bound) override {

		this->upper_bound = upper_bound;
		items.SetBounds(left_bound, upper_bound);
	}

	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override {
		XSize new_size = vertical_ending_line.GetSizeWithoutLine(size);

		int y_end = composer.Draw(gc, initial_coords, new_size);

		vertical_ending_line.Draw(gc, initial_coords, \
									XSize(size.width, \
											y_end - initial_coords.y));
		
		vertical_ending_line.FillEmptySpace(gc, initial_coords.x, y_end, \
							initial_coords.x + size.width, \
							initial_coords.y + size.height);
	}

	void SetItemHeight(const int height) override {

		composer.SetRecordHeight(height);
	}
	
	void AcceptConfigurator(IConfigurator *configurator) override {

		configurator->Configure(vertical_ending_line);
		configurator->Configure(items);
	}

	void BeginVScroll(const int scroll_pos) override {

		composer.OnScroll(scroll_pos);
		items.BeginScroll(scroll_pos);
	}

	void BeginHScroll(const int scroll_pos) override {
	
		items.BeginScroll(scroll_pos);
	}

	void OnClick(const int x, const int y) override {
		size_t record = composer.GetItemIndexByCoord(y - upper_bound);

		if (record < data.size())
			items.OnClick(0, record);
	}

	inline void OnClick(const int x, const int y, const size_t field) {
		size_t record = composer.GetItemIndexByCoord(y - upper_bound);

		if (record < data.size())
			items.OnClick(field, record);
	}

	size_t GetVisibleRecordsCount(const int height) const override {

		return composer.GetVisibleItemsCount(height);
	}

	int FocusOnRecord(const size_t record_index, const int height) override {

		return composer.FocusOnItem(record_index, height);
	}

	int FocusOnField(const size_t field, const int grid_widget_width, \
					const int sum) override {

		return 0;
	}

	int RemoveField(const size_t field, const int grid_widget_width, \
					const int sum) override {
	
		items.OnFieldRemoved(field);
		return 0;
	}

	virtual ~TSimpleFieldLayout(){ }
};

template <typename Data, typename Records> \
class TFieldOfRecordsLayout final : public TCellsLayout<Data, Records>, \
								public ISharedComposerContainer{
	CFieldComposer composer;
	int upper_bound;
	CVerticalLine line;

	TFieldOfRecordsLayout(const TFieldOfRecordsLayout &obj) = delete;
	TFieldOfRecordsLayout &operator=(const TFieldOfRecordsLayout &obj) = delete;
	TFieldOfRecordsLayout &operator=(TFieldOfRecordsLayout &&obj) = delete;

	inline Records CreateRecords(Data data_, \
								std::shared_ptr<const ISizes> fields_sizes_) {

		if (data_.size()) return Records(data_[0], fields_sizes_);
		return Records(fields_sizes_);
	}
public:
	TFieldOfRecordsLayout(Data data_, std::shared_ptr<const ISizes> fields_sizes_) : \
			TCellsLayout(data_, CreateRecords(data_, fields_sizes_)), \
			composer(this), upper_bound(0){ }

	TFieldOfRecordsLayout(TFieldOfRecordsLayout &&obj) : \
			TCellsLayout(std::move(obj)), \
			composer(this, std::move(obj.composer)), \
			upper_bound(obj.upper_bound){
	
		obj.upper_bound = 0;
	}

	const CRecordComposer getComposer() const override{

		return items.getComposer();
	}

	IDrawable &GetByIndex(const size_t index) override {

		items.Init(data[index]);
		return items;
	}

	void SetBounds(const int left_bound, const int upper_bound) override {

		this->upper_bound = upper_bound;
		items.SetBounds(left_bound, upper_bound);
	}

	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override {

		int y_end = composer.Draw(gc, initial_coords, size);

		line.FillEmptySpace(gc, initial_coords.x, y_end, \
							initial_coords.x + size.width, \
							initial_coords.y + size.height);
	}

	void SetItemHeight(const int height) override {

		composer.SetRecordHeight(height);
	}

	void AcceptConfigurator(IConfigurator *configurator) override {

		configurator->Configure(line);
		items.AcceptConfigurator(configurator);
	}

	void BeginVScroll(const int scroll_pos) override { 
	
		items.BeginVScroll(scroll_pos);
		composer.OnScroll(scroll_pos);
	}

	void BeginHScroll(const int scroll_pos) override {

		items.BeginHScroll(scroll_pos);
	}

	void OnClick(const int x, const int y) override {
		size_t record = composer.GetItemIndexByCoord(y - upper_bound);

		if(record < data.size())
			items.OnClick(x, y, record);
	}

	size_t GetVisibleRecordsCount(const int height) const override {

		return composer.GetVisibleItemsCount(height);
	}

	int FocusOnRecord(const size_t record_index, const int height) override {

		return composer.FocusOnItem(record_index, height);
	}

	int FocusOnField(const size_t field, const int grid_widget_width, \
					const int sum) override {

		return items.FocusOnField(field, grid_widget_width, sum);
	}

	int RemoveField(const size_t field, const int grid_widget_width, \
					const int sum) override {

		return items.RemoveField(field, grid_widget_width, sum);
	}

	virtual ~TFieldOfRecordsLayout(){ }
};

template <typename Data, typename Cells> \
class TSimpleRecordLayout : \
			public TCellsLayout<Data, TCellWithLine<Cells, CVerticalLine> >, \
			public ISharedComposerContainer{
	CRecordComposer composer;
	int left_bound;
	CHorizontalLine horizontal_ending_line;
	
	TSimpleRecordLayout(const TSimpleRecordLayout &obj) = delete;
	TSimpleRecordLayout &operator=(const TSimpleRecordLayout &obj) = delete;
	TSimpleRecordLayout &operator=(TSimpleRecordLayout &&obj) = delete;
public:
	TSimpleRecordLayout(Data data_, std::shared_ptr<const ISizes> fields_sizes_) : \
			TCellsLayout(data_), \
			composer(this, fields_sizes_), left_bound(0) { }

	TSimpleRecordLayout(std::shared_ptr<const ISizes> fields_sizes_) : \
			composer(this, fields_sizes_), left_bound(0) { }

	TSimpleRecordLayout(Data data_, const CRecordComposer &shared_composer) : \
			TCellsLayout(data_), \
			composer(this, shared_composer), left_bound(0) { }

	TSimpleRecordLayout(Data data_, CRecordComposer &&shared_composer) : \
			TCellsLayout(data_), \
			composer(this, std::move(shared_composer)), \
			left_bound(0) { }

	TSimpleRecordLayout(TSimpleRecordLayout &&obj) : \
			TCellsLayout(std::move(obj)), \
			composer(std::move(obj.composer)), \
			left_bound(obj.left_bound), \
			horizontal_ending_line(std::move(obj.horizontal_ending_line)) {
	
		obj.left_bound = 0;
		composer.Init(this);		
	}

	const CRecordComposer getComposer() const override{

		return composer;
	}

	IDrawable &GetByIndex(const size_t index) override {
		std::pair<size_t, size_t> field_and_record = data.GetCellCoords(index);

		items.Init(data[index], field_and_record.first, field_and_record.second);
		return items;
	}

	void SetBounds(const int left_bound, const int upper_bound) override {

		this->left_bound = left_bound;
		items.SetBounds(left_bound, upper_bound);
	}

	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override {
		XSize new_size = horizontal_ending_line.GetSizeWithoutLine(size);

		int x_end = composer.Draw(gc, initial_coords, new_size);

		horizontal_ending_line.Draw(gc, initial_coords, \
									XSize(x_end - initial_coords.x, \
											size.height));

		horizontal_ending_line.FillEmptySpace(gc, x_end, initial_coords.y, \
								initial_coords.x + size.width, \
								initial_coords.y + size.height);
	}

	void SetItemHeight(const int height) override {	}

	void AcceptConfigurator(IConfigurator *configurator) override {

		configurator->Configure(horizontal_ending_line);
		configurator->Configure(items);
	}

	void BeginVScroll(const int scroll_pos) override { 
	
		items.BeginScroll(scroll_pos);
	}

	void BeginHScroll(const int scroll_pos) override {

		items.BeginScroll(scroll_pos);
		composer.OnScroll(scroll_pos);
	}

	void OnClick(const int x, const int y) override {
		size_t field = composer.GetItemIndexByCoord(x - left_bound);

		if (field != CRecordComposer::NULL_ITEM_INDEX)
			items.OnClick(field, 0);
	}

	void OnClick(const int x, const int y, const size_t record) {
		size_t field = composer.GetItemIndexByCoord(x - left_bound);

		if (field != CRecordComposer::NULL_ITEM_INDEX)
			items.OnClick(field, record);
	}

	size_t GetVisibleRecordsCount(const int height) const override { return 0; }

	int FocusOnRecord(const size_t record_index, const int height) override { return 0; }

	int FocusOnField(const size_t field, const int grid_widget_width, \
					const int sum) override {

		return composer.FocusOnField(field, grid_widget_width, sum);
	}

	int RemoveField(const size_t field, const int grid_widget_width, \
					const int sum) override {

		items.OnFieldRemoved(field);
		return composer.RemoveField(field, grid_widget_width, sum);
	}

	virtual ~TSimpleRecordLayout(){ }
};

template <typename Data, typename Fields> \
class TRecordOfFieldsLayout final : \
				public TCellsLayout<Data, Fields>, \
				public ISharedComposerContainer{
	CRecordComposer composer;
	int left_bound;
	CHorizontalLine line;

	TRecordOfFieldsLayout(const TRecordOfFieldsLayout &obj) = delete;
	TRecordOfFieldsLayout &operator=(const TRecordOfFieldsLayout &obj) = delete;
	TRecordOfFieldsLayout &operator=(TRecordOfFieldsLayout &&obj) = delete;

	inline Fields CreateFields(Data data_) {

		if (data_.size()) return Fields(data_[0]);
		return Fields();
	}
public:
	TRecordOfFieldsLayout(Data data_, std::shared_ptr<const ISizes> fields_sizes_) : \
			TCellsLayout(data_, CreateFields(data_)), \
			composer(this, fields_sizes_), left_bound(0){ }

	TRecordOfFieldsLayout(TRecordOfFieldsLayout &&obj) : \
			TCellsLayout(std::move(obj)), \
			composer(std::move(obj.composer)), \
			left_bound(obj.left_bound) {
			
		obj.left_bound = 0;
		composer.Init(this);
	}

	const CRecordComposer getComposer() const {

		return composer;
	}

	IDrawable &GetByIndex(const size_t index) override {

		items.Init(data[index]);
		return items;
	}

	void SetBounds(const int left_bound, const int upper_bound) override {

		this->left_bound = left_bound;
		items.SetBounds(left_bound, upper_bound);
	}

	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override {
		int x_end = composer.Draw(gc, initial_coords, size);

		line.FillEmptySpace(gc, x_end, initial_coords.y, \
							initial_coords.x + size.width, \
							initial_coords.y + size.height);
	}

	void SetItemHeight(const int height) override {

		items.SetItemHeight(height);
	}

	void AcceptConfigurator(IConfigurator *configurator) override {

		configurator->Configure(line);
		items.AcceptConfigurator(configurator);
	}

	void BeginVScroll(const int scroll_pos) override {

		items.BeginVScroll(scroll_pos);
	}

	void BeginHScroll(const int scroll_pos) override {

		items.BeginHScroll(scroll_pos);
		composer.OnScroll(scroll_pos);
	}

	void OnClick(const int x, const int y) override {
		size_t field = composer.GetItemIndexByCoord(x - left_bound);

		if (field != CRecordComposer::NULL_ITEM_INDEX)
			items.OnClick(x, y, field);
	}

	size_t GetVisibleRecordsCount(const int height) const override {

		return items.GetVisibleRecordsCount(height);
	}

	int FocusOnRecord(const size_t record_index, const int height) override {

		return items.FocusOnRecord(record_index, height);
	}

	int FocusOnField(const size_t field, const int grid_widget_width, \
					const int sum) override {

		items.FocusOnField(field, grid_widget_width, sum);
		return composer.FocusOnField(field, grid_widget_width, sum);
	}

	int RemoveField(const size_t field, const int grid_widget_width, \
					const int sum) override {

		items.RemoveField(field, grid_widget_width, sum);
		return composer.RemoveField(field, grid_widget_width, sum);
	}

	virtual ~TRecordOfFieldsLayout(){ }
};
