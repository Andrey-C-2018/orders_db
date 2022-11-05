#pragma once

struct IUploadNotifier {

	virtual void step() = 0;
	virtual void completed() = 0;

	virtual ~IUploadNotifier() { }
};