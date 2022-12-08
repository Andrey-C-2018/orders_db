#pragma once
#include <memory>
#include <basic/IProperties.h>

class CPropertiesFile;

class CFlexiblePropepties : public IProperties {
	std::unique_ptr<CPropertiesFile> per_machine, per_user;

public:
	CFlexiblePropepties();

	CFlexiblePropepties(const CFlexiblePropepties &obj) = delete;
	CFlexiblePropepties(CFlexiblePropepties &&obj) = default;
	CFlexiblePropepties &operator=(const CFlexiblePropepties &obj) = delete;
	CFlexiblePropepties &operator=(CFlexiblePropepties &&obj) = default;

	void open();
	const Tchar *getStringProperty(const Tchar *name, Tstring &buffer) const override;
	const Tchar *getStringPropertyThreadUnsafe(const Tchar *name) override;
	int getIntProperty(const Tchar *name, Tstring &buffer) const override;
	int getIntProperty(const Tchar *name, Tstring &buffer, bool &not_found) const override;

	virtual ~CFlexiblePropepties();
};

