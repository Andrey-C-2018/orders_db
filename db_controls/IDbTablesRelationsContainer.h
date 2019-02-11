#pragma once

struct IDbTablesRelationsContainer {
	
	virtual void SetRelation(const char *master_field, const char *dependent_field) = 0;
	~IDbTablesRelationsContainer() { }
};