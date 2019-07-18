#pragma once

struct ITabKeyAction {

	virtual void OnTabKeyPressed() = 0;
	virtual ~ITabKeyAction() { }
};

