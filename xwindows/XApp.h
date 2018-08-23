#pragma once

class XApp{
protected:
	static XApp *instance;
	bool msg_loop_status;
	int return_code;
	XApp() noexcept : msg_loop_status(false), \
						return_code(-1){ }
public:
	static XApp *GetInstance();
	virtual void OnInit();
	inline bool IsMessageLoopLaunched() const {
		return msg_loop_status;
	}
	inline bool IsQuitMessagePosted() const {
		return return_code != -1;
	}
	int LaunchMessageLoop();
	void Destroy();
	virtual ~XApp() noexcept;
};

#define DECLARE_APP \
public: \
	static bool CreateInstance();

#define IMPLEMENT_APP(XAppDerived) \
bool ignored = XAppDerived::CreateInstance(); \
\
bool XAppDerived::CreateInstance(){ \
\
if(!instance) \
	instance = new XAppDerived(); \
return true; \
}

// *********************************************