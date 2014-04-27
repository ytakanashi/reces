//CtrlAHook.h


#ifndef _CTRLAHOOK_H_2B3F5C00B309_4df9_B4E2_0EEF0277B861
#define _CTRLAHOOK_H_2B3F5C00B309_4df9_B4E2_0EEF0277B861
#include<windows.h>

namespace sslib{
class CtrlAHook{
public:
	CtrlAHook();
	virtual ~CtrlAHook();

private:
	HHOOK m_hook;
	static LRESULT CALLBACK CtrlAHookProc(int,WPARAM,LPARAM);
};

//namespace sslib
}

#endif //_CTRLAHOOK_H_2B3F5C00B309_4df9_B4E2_0EEF0277B861
