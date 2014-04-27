//Window.cpp
//ウインドウクラス


#include"../StdAfx.h"
#include"sslib.h"


namespace sslib{

Window::WindowProcBase* Window::m_window_proc;
Window::handlers Window::m_message_handlers;

Window::Window(HINSTANCE instance_handle,WNDCLASSEX* wcx,bool use_peek_message,bool no_window_proc):
	m_instance_handle(instance_handle),
	m_wnd_handle(NULL),
	m_class_name(),
	m_exit_code(EXIT_SUCCESS),
	m_use_peek_message(use_peek_message){
		if(!no_window_proc)m_window_proc=new WindowProc<Window>(this,&Window::messageRouter);

		if(wcx){
			m_class_name=wcx->lpszClassName?wcx->lpszClassName:wcx->lpszClassName=_T("Window_Class_Name");
			registerWindow(wcx);
		}
}

Window::~Window(){
	closeWindow();

	if(m_window_proc){
		delete m_window_proc;

		handlers::iterator ite_handlers=m_message_handlers.find(this);
		if(ite_handlers!=m_message_handlers.end()){
			for(handler_base::iterator ite_handler_base=ite_handlers->second.begin(),
				end=ite_handlers->second.end();
				ite_handler_base!=end;
				++ite_handler_base){
				//ハンドラを削除
				delete ite_handler_base->second;
			}
			//オブジェクトの登録自体を削除
//			m_message_handlers.erase(ite_handlers);
		}
	}
//	::UnregisterClass(m_class_name.c_str(),m_instance_handle);
}
/*
LRESULT Window::onDestroy(WPARAM wparam,LPARAM lparam){
	::PostQuitMessage(0);
	return 0;
}
*/
void Window::onIdle(){
}

void Window::unregisterMessageHandler(UINT message){
	handlers::iterator ite_handlers=m_message_handlers.find(this);
	if(ite_handlers!=m_message_handlers.end()){
		if(ite_handlers->second.find(message)!=ite_handlers->second.end()){
			//ハンドラを削除
			delete m_message_handlers[this][message];
			//イベントを削除
			ite_handlers->second.erase(message);
			//そのオブジェクトが登録しているメッセージハンドラが0であればオブジェクトの登録自体を削除
			if(!ite_handlers->second.size())m_message_handlers.erase(ite_handlers);
		}
	}
}
/*
LRESULT CALLBACK Window::messageRouter(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam){
	Window* wnd=0;

	if(message==WM_NCCREATE){
		wnd=reinterpret_cast<Window*>(((LPCREATESTRUCT)lparam)->lpCreateParams);
		::SetWindowLong(wnd_handle,GWL_USERDATA,reinterpret_cast<long>(wnd));

		wnd->registerMessageHandler<Window>(WM_DESTROY,wnd,&Window::onDestroy);
	}else{
		wnd=reinterpret_cast<Window*>(::GetWindowLong(wnd_handle,GWL_USERDATA));
	}

	if(wnd){
		handlers::iterator ite_handlers=m_message_handlers.find(wnd);
		if(ite_handlers!=m_message_handlers.end()){
			if(ite_handlers->second.find(message)!=ite_handlers->second.end()){
				return m_message_handlers[wnd][message]->operator()(wparam,lparam);
			}
		}
	}
	return ::DefWindowProc(wnd_handle,message,wparam,lparam);
}
*/


LRESULT Window::messageRouter(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam){
	handlers::iterator ite_handlers=m_message_handlers.find(this);
	if(ite_handlers!=m_message_handlers.end()){
		if(ite_handlers->second.find(message)!=ite_handlers->second.end()){
			return m_message_handlers[this][message]->operator()(wparam,lparam);
		}
	}
	return ::DefWindowProc(wnd_handle,message,wparam,lparam);
}

LRESULT CALLBACK Window::windowProc(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam){
	return m_window_proc->operator()(wnd_handle,message,wparam,lparam);
}

LRESULT CALLBACK Window::dummyProc(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam){
	return ::DefWindowProc(wnd_handle,message,wparam,lparam);
}

bool Window::handleMessages(){
	MSG msg;

	if(!m_wnd_handle)return false;

	if((m_use_peek_message)?
	   ::PeekMessage(&msg,NULL,0,0,PM_REMOVE):
	   ::GetMessage(&msg,NULL,0,0)){
		if(m_use_peek_message&&msg.message==WM_QUIT){
			setExitCode(msg.wParam);
			return false;
		}
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}else{
		if(m_use_peek_message){
			onIdle();
		}else{
			setExitCode(msg.wParam);
			return false;
		}
	}

	return true;
}

bool Window::registerWindow(LPCTSTR class_name,UINT style,HICON icon,HCURSOR cursor,HICON small_icon,HBRUSH background,LPCTSTR menu_name){
	WNDCLASSEX wcx={0};

	wcx.cbSize=sizeof(WNDCLASSEX);
	wcx.style=style;
	wcx.cbClsExtra=0;
	wcx.cbWndExtra=0;
	wcx.hIcon=icon;
	wcx.hCursor=cursor;
	wcx.hbrBackground=background;
	wcx.lpfnWndProc=(m_window_proc)?(Window::windowProc):(Window::dummyProc);
	wcx.hInstance=m_instance_handle;
	wcx.lpszClassName=(class_name)?class_name:_T("Window_Class_Name");
	wcx.lpszMenuName=menu_name;
	wcx.hIconSm=small_icon;

	m_class_name=wcx.lpszClassName;

	return ::RegisterClassEx(&wcx)!=0;
}

bool Window::registerWindow(WNDCLASSEX* wcx){
	if(wcx==NULL)return false;

	wcx->cbSize=sizeof(WNDCLASSEX);
	wcx->lpfnWndProc=(m_window_proc)?(Window::windowProc):(Window::dummyProc);
	wcx->lpszClassName=(wcx->lpszClassName)?wcx->lpszClassName:_T("Window_Class_Name");

	m_class_name=wcx->lpszClassName;

	return ::RegisterClassEx(wcx)!=0;
}

bool Window::createWindow(DWORD style,DWORD style_ex,const TCHAR* wnd_name,int x,int y,int width,int height,HWND parent){
	m_wnd_handle=::CreateWindowEx(style_ex,
								m_class_name.c_str(),
								wnd_name,
								parent?WS_CHILD|style:WS_OVERLAPPEDWINDOW|style,
								x,y,width,height,
								parent,
								NULL,
								NULL,
								NULL);
/*
	if(m_wnd_handle!=NULL&&m_window_proc){
		this->registerMessageHandler<Window>(WM_DESTROY,this,&Window::onDestroy);
	}
*/
	return m_wnd_handle!=NULL;
}

bool Window::showWindow(int cmd){
	bool result=::ShowWindow(wnd(),cmd)!=0;
	::UpdateWindow(wnd());
	return result;
}

//namespace sslib
}
