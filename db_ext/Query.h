#pragma once

class CQuery{
	CMetaInfo meta_info;
	std::shared_ptr<IDbStatement> stmt;
public:
	CQuery();
	
	const CMetaInfo &getMetaInfo() const;
	void bindInt();
	
	std::shared_ptr<IDbResultSet> exec();
};