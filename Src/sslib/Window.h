//Window.h

#ifndef _WINDOW_H_E78D3DA7_19C0_489a_B854_C84AB4A5A2EF
#define _WINDOW_H_E78D3DA7_19C0_489a_B854_C84AB4A5A2EF

namespace sslib{
class Window{
public:
	Window(HINSTANCE instance_handle=::GetModuleHandle(NULL),
		   WNDCLASSEX* wcx=NULL,
		   bool use_peek_message=false,
		   bool no_window_proc=false);
	virtual ~Window();

class WindowProcBase{
public:
	virtual LRESULT operator()(HWND,UINT,WPARAM,LPARAM)=0;
	virtual ~WindowProcBase(){}
};
template<class T>
class WindowProc:public WindowProcBase{
public:
	typedef LRESULT(T::*Func)(HWND,UINT,WPARAM,LPARAM);
private:
	T* m_class_ptr;
	Func m_func;
public:
	WindowProc(T* class_ptr,Func func):m_class_ptr(class_ptr),m_func(func){}
	virtual LRESULT operator()(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam){
		return (m_class_ptr->*m_func)(wnd_handle,message,wparam,lparam);
	}
};

class Handler{
public:
	virtual LRESULT operator()(WPARAM wparam,LPARAM lparam)=0;
	virtual ~Handler(){}
};
template<class T>
class MessageHandler:public Handler{
public:
	typedef LRESULT(T::*Func)(WPARAM,LPARAM);
private:
	T* m_class_ptr;
	Func m_func;
public:
	MessageHandler(T* class_ptr,Func func):m_class_ptr(class_ptr),m_func(func){}
	virtual LRESULT operator()(WPARAM wparam,LPARAM lparam){
		return (m_class_ptr->*m_func)(wparam,lparam);
	}
};

private:
	LRESULT messageRouter(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam);
	static LRESULT CALLBACK windowProc(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam);
	static LRESULT CALLBACK dummyProc(HWND wnd_handle,UINT message, WPARAM wparam,LPARAM lparam);
	LRESULT onDestroy(WPARAM wparam,LPARAM lparam);

protected:
	HINSTANCE m_instance_handle;
	HWND m_wnd_handle;
	RECT m_rect;

	//内部の関数を呼び出すだけ
	static WindowProcBase* m_window_proc;

	typedef std::map<UINT,Handler*> handler_base;
	typedef std::map<Window*,handler_base> handlers;
	//外部の関数を呼び出す
	static handlers m_message_handlers;

	virtual void onIdle();

	tstring m_class_name;

	int m_exit_code;
	bool m_use_peek_message;

public:
	template<class T,typename T1,typename T2>
	inline void registerMessageHandler(UINT message,T1* t1,T2 t2){
		bool(WINAPI*p_changeWindowMessageFilter)(UINT,DWORD)=(bool(WINAPI*)(UINT,DWORD))::GetProcAddress(::LoadLibrary(_T("user32.dll")),"ChangeWindowMessageFilter");
		if(p_changeWindowMessageFilter!=NULL){
			//MSGFLT_ADD
			p_changeWindowMessageFilter(message,1);
		}

		m_message_handlers[this][message]=new MessageHandler<T>(t1,t2);
	}

	void unregisterMessageHandler(UINT message);

	bool handleMessages();

	virtual bool registerWindow(const TCHAR* class_name=NULL,
								UINT style=CS_HREDRAW|CS_VREDRAW,
								HICON icon=::LoadIcon(NULL,IDI_APPLICATION),
								HCURSOR cursor=::LoadCursor(NULL,IDC_ARROW),
								HICON small_icon=::LoadIcon(NULL,IDI_APPLICATION),
								HBRUSH background=(HBRUSH)(COLOR_BTNFACE+1),
								LPCTSTR menu_name=NULL);

	virtual bool registerWindow(WNDCLASSEX* wcx);
	virtual bool createWindow(DWORD style=WS_OVERLAPPEDWINDOW,
						DWORD style_ex=0,
						const TCHAR* wnd_name=NULL,
						int x=CW_USEDEFAULT,
						int y=CW_USEDEFAULT,
						int width=CW_USEDEFAULT,
						int height=CW_USEDEFAULT,
						HWND parent=NULL);
	bool showWindow(int cmd=SW_SHOW);
	inline void closeWindow(){DestroyWindow(m_wnd_handle);}//postMessage(WM_CLOSE);}

	inline HWND wnd()const{return m_wnd_handle;}
	inline HINSTANCE inst()const{return m_instance_handle;}
	inline void setHandle(HWND wnd_handle){m_wnd_handle=wnd_handle;}
	inline int getExitCode()const{return m_exit_code;}
	inline void setExitCode(int code){m_exit_code=code;}

	LRESULT sendMessage(UINT message,WPARAM wparam=0,LPARAM lparam=0);
	bool postMessage(UINT message,WPARAM wparam=0,LPARAM lparam=0);
};

	inline LRESULT Window::sendMessage(UINT message,WPARAM wparam,LPARAM lparam){
		return ::SendMessage(m_wnd_handle,message,wparam,lparam);
	}
	inline bool Window::postMessage(UINT message,WPARAM wparam,LPARAM lparam){
		return ::PostMessage(m_wnd_handle,message,wparam,lparam)!=0;
	}

//namespace sslib
}

#endif //_WINDOW_H_E78D3DA7_19C0_489a_B854_C84AB4A5A2EF
