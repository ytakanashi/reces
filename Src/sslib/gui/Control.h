//Control.h

#ifndef _CONTROL_H_2E7EAF9F_5EF0_40a2_88E1_1E43EF49EBF4
#define _CONTROL_H_2E7EAF9F_5EF0_40a2_88E1_1E43EF49EBF4

namespace sslib{
class Control{
public:
	Control(HWND parent_handle,UINT resource_id,bool sub_class=false);

	virtual ~Control(){
		if(m_sub_class){
			::SetWindowLongPtr(handle(),GWL_WNDPROC,reinterpret_cast<LONG_PTR>(m_default_proc));
		}
	}

private:
	UINT m_resource_id;
	HWND m_parent_handle;
	HWND m_handle;
	bool m_sub_class;

protected:
	WNDPROC m_default_proc;

protected:
	virtual LRESULT onNotify(WPARAM wparam,LPARAM lparam);
	virtual LRESULT onMessage(UINT message,WPARAM wparam,LPARAM lparam);

public:
	virtual LRESULT handleMessage(HWND dlg_handle,UINT message,WPARAM wparam,LPARAM lparam);

	LRESULT sendMessage(UINT message,WPARAM wparam=0,LPARAM lparam=0);
	bool postMessage(UINT message,WPARAM wparam=0,LPARAM lparam=0);

	UINT resource_id()const{return m_resource_id;}
	HWND parent_handle()const{return m_parent_handle;}
	HWND handle()const{return m_handle;}
	WNDPROC default_proc()const{return m_default_proc;}
};

//namespace sslib
}

#endif //_CONTROL_H_2E7EAF9F_5EF0_40a2_88E1_1E43EF49EBF4
