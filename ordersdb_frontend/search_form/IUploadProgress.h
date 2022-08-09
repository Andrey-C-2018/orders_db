#pragma once

struct IUploadProgress {

	virtual void step() = 0;
	virtual void completed() = 0;

	virtual ~IUploadProgress() { }
};