#pragma once

struct IConfigurator;

struct IInteractiveObject {
	virtual void AcceptConfigurator(IConfigurator *configurator) = 0;
	virtual void BeginVScroll(const int scroll_pos) = 0;
	virtual void BeginHScroll(const int scroll_pos) = 0;
	virtual void EndScroll(const int scroll_pos) = 0;
	virtual void OnClick(const int x, const int y) = 0;
	virtual ~IInteractiveObject() { }
};