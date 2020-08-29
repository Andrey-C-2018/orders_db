#pragma once
#include <memory>
#include <db/IDbBindingTarget.h>

class CDoubleBndTarget : public IDbBindingTarget {
	std::shared_ptr<IDbBindingTarget> target1, target2;

public:
	CDoubleBndTarget();
	CDoubleBndTarget(std::shared_ptr<IDbBindingTarget> target1_, \
						std::shared_ptr<IDbBindingTarget> target2_);

	CDoubleBndTarget(const CDoubleBndTarget &obj) = default;
	CDoubleBndTarget(CDoubleBndTarget &&obj) = default;
	CDoubleBndTarget &operator=(const CDoubleBndTarget &obj) = default;
	CDoubleBndTarget &operator=(CDoubleBndTarget &&obj) = default;

	inline void replaceFirst(std::shared_ptr<IDbBindingTarget> target1);
	inline void replaceSecond(std::shared_ptr<IDbBindingTarget> target2);

	inline std::shared_ptr<IDbBindingTarget> getFirst() {

		return target1;
	}

	void bindValue(const size_t param_no, const int value) override;

	void bindValue(const size_t param_no, const char *value) override;
	void bindValue(const size_t param_no, const wchar_t *value) override;
	void bindValue(const size_t param_no, \
					const ImmutableString<char> value) override;
	void bindValue(const size_t param_no, \
					const ImmutableString<wchar_t> value) override;

	void bindValue(const size_t param_no, const CDate &value) override;
	void bindNull(const size_t param_no) override;

	virtual ~CDoubleBndTarget();
};

//*****************************************************

void CDoubleBndTarget::replaceFirst(std::shared_ptr<IDbBindingTarget> target1) {

	assert(target1);
	this->target1 = target1;
}

void CDoubleBndTarget::replaceSecond(std::shared_ptr<IDbBindingTarget> target2) {

	assert(target2);
	this->target2 = target2;
}