//Control.cpp
//コントロール(リソースファイル内限定)クラス


#include"../../StdAfx.h"
#include"../sslib.h"


namespace sslib{

LRESULT CALLBACK controlMessageRouter(HWND handle,UINT message,WPARAM wparam,LPARAM lparam){
	//インスタンスのポインタを取り出す
	Control* ctrl=reinterpret_cast<Control*>(::GetWindowLongPtr(handle,GWLP_USERDATA));

	if(ctrl!=NULL){
		return ctrl->handleMessage(handle,message,wparam,lparam);
//		return ::CallWindowProc(ctrl->default_proc(),handle,message,wparam,lparam);
	}
	return 0;
}

Control::Control(HWND parent_handle,UINT resource_id,bool sub_class):
	m_resource_id(resource_id),
	m_parent_handle(parent_handle),
	m_handle(::GetDlgItem(parent_handle,resource_id)),
	m_sub_class(sub_class),
	m_default_proc(NULL){
	if(m_sub_class){
		//インスタンスのポインタを保存
		::SetWindowLongPtr(handle(),GWLP_USERDATA,reinterpret_cast<LONG_PTR>(this));

		//サブクラス化
		m_default_proc=reinterpret_cast<WNDPROC>(::GetWindowLongPtr(handle(),GWLP_WNDPROC));
		::SetWindowLongPtr(handle(),GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(controlMessageRouter));
	}
}

LRESULT Control::onNotify(WPARAM wparam,LPARAM lparam){
	return 0;
}

LRESULT Control::onMessage(UINT message,WPARAM wparam,LPARAM lparam){
	return 0;
}

LRESULT Control::handleMessage(HWND handle,UINT message,WPARAM wparam,LPARAM lparam){
	if(message==WM_NOTIFY){
		return onNotify(wparam,lparam);
	}else{
		return onMessage(message,wparam,lparam);
	}
}

LRESULT Control::sendMessage(UINT message,WPARAM wparam,LPARAM lparam){
	return ::SendMessage(handle(),message,wparam,lparam);
}

bool Control::postMessage(UINT message,WPARAM wparam,LPARAM lparam){
	return ::PostMessage(handle(),message,wparam,lparam)!=0;
}



//namespace sslib
}
