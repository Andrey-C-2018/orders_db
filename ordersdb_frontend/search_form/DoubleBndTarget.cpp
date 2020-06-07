#include "DoubleBndTarget.h"

CDoubleBndTarget::CDoubleBndTarget() { }

CDoubleBndTarget::CDoubleBndTarget(std::shared_ptr<IDbBindingTarget> target1_, \
									std::shared_ptr<IDbBindingTarget> target2_) : \
								target1(target1_), target2(target2_) { 

	assert(target1);
	assert(target2);
}

void CDoubleBndTarget::bindValue(const size_t param_no, const int value) {

	target1->bindValue(param_no, value);
	target2->bindValue(param_no, value);
}

void CDoubleBndTarget::bindValue(const size_t param_no, const char *value) {

	target1->bindValue(param_no, value);
	target2->bindValue(param_no, value);
}

void CDoubleBndTarget::bindValue(const size_t param_no, const wchar_t *value) {

	target1->bindValue(param_no, value);
	target2->bindValue(param_no, value);
}

void CDoubleBndTarget::bindValue(const size_t param_no, \
									const ImmutableString<char> value) {

	target1->bindValue(param_no, value);
	target2->bindValue(param_no, value);
}

void CDoubleBndTarget::bindValue(const size_t param_no, \
									const ImmutableString<wchar_t> value) {

	target1->bindValue(param_no, value);
	target2->bindValue(param_no, value);
}

void CDoubleBndTarget::bindValue(const size_t param_no, const CDate &value) {

	target1->bindValue(param_no, value);
	target2->bindValue(param_no, value);
}

void CDoubleBndTarget::bindNull(const size_t param_no) {

	target1->bindNull(param_no);
	target2->bindNull(param_no);
}

CDoubleBndTarget::~CDoubleBndTarget() { }
