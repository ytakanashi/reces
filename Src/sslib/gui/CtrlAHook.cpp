//CtrlAHook.cpp
//Ctrl+Aをローカルフックして、エディットコントロールの文字列を全選択できるようにする


#include"../../StdAfx.h"
#include"../sslib.h"


namespace sslib{

//コンストラクタ
CtrlAHook::CtrlAHook(){
	m_hook=::SetWindowsHookEx(WH_MSGFILTER,CtrlAHookProc,NULL,::GetCurrentThreadId());
}

//デストラクタ
CtrlAHook::~CtrlAHook(){
	::UnhookWindowsHookEx(m_hook);
	m_hook=NULL;
}

LRESULT CALLBACK CtrlAHook::CtrlAHookProc(int nCode,WPARAM wParam,LPARAM lParam){
	if(nCode<0)return ::CallNextHookEx(NULL/*This parameter is ignored.(by MSDN)*/,nCode,wParam,lParam);

	MSG *pMsg=(MSG*)lParam;
	TCHAR class_name[128];
	HWND hwnd;

	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam=='A'&&
		   ::GetAsyncKeyState(VK_CONTROL)<0&&
		   ::GetAsyncKeyState(VK_SHIFT)>=0&&
		   ::GetAsyncKeyState(VK_MENU)>=0){
			//Ctrl+Aが押下されている場合
			hwnd=::GetFocus();
			if(hwnd!=NULL){
				::GetClassName(hwnd,class_name,ARRAY_SIZEOF(class_name));
				if(lstrcmpi(class_name,_T("Edit"))==0){
					//フォーカスがエディットコントロールにある
					::SendMessage(hwnd,EM_SETSEL,0,-1);
					return true;
				}
			}
		}
	}
	return ::CallNextHookEx(NULL/*This parameter is ignored.(by MSDN)*/,nCode,wParam,lParam);
}

//namespace sslib
}
