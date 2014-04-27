//WindowApp.h

#ifndef _WINDOWAPP_H_E3B52ED2_7A1B_46d7_83F6_0148C1B16FE2
#define _WINDOWAPP_H_E3B52ED2_7A1B_46d7_83F6_0148C1B16FE2


extern int WINAPI WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show);

namespace sslib{
class WindowApp{
	friend WindowApp* app();
	friend int WINAPI ::WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show);

protected:
	WindowApp():m_main_wnd(NULL){
		this_ptr=this;
	}

	virtual ~WindowApp(){
	}

private:
	Window* m_main_wnd;
	static WindowApp* this_ptr;

protected:
	virtual bool init();
	virtual bool run(CommandArgument& cmd_arg,int cmd_show)=0;
	virtual void cleanup();

public:
	inline Window* getMainWnd()const{return m_main_wnd;}
	inline HWND getMainWndHandle()const{return m_main_wnd?m_main_wnd->wnd():NULL;}
	inline void setMainWnd(Window* wnd){m_main_wnd=wnd;}
};

//namespace sslib
}

#endif //_WINDOWAPP_H_E3B52ED2_7A1B_46d7_83F6_0148C1B16FE2
