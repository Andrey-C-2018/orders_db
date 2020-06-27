#pragma once
#include <map>
#include <xwindows/XComboBox.h>

class CMetaInfo;
class CSortingManager;
class CGrid;

class COrderingComboBox : public XComboBox {
	std::map<size_t, size_t> ordering_map;
	CSortingManager *manager;
	Tstring buffer;

	void OnItemChoosed(XCommandEvent *eve);

public:
	COrderingComboBox(CSortingManager *manager_);

	COrderingComboBox(const COrderingComboBox &obj) = delete;
	COrderingComboBox(COrderingComboBox &&obj) = default;
	COrderingComboBox &operator=(const COrderingComboBox &obj) = delete;
	COrderingComboBox &operator=(COrderingComboBox &&obj) = default;

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	void addOrderingField(const char *field_name, const CMetaInfo &meta_info, \
							const CGrid *grid);

	virtual ~COrderingComboBox();
};

