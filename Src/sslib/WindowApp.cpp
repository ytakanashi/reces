//WindowApp.cpp
//GUIなアプリ専用汎用クラス

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{

WindowApp* WindowApp::this_ptr=NULL;

WindowApp* app(){
	return WindowApp::this_ptr;
}

bool WindowApp::init(){
	return true;
}

void WindowApp::cleanup(){
}

//namespace sslib
}

using namespace sslib;

//GUIなアプリのエントリポイント
int WINAPI WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show){
	createInstance();
	WindowApp* app_ptr=app();
	app_ptr->m_main_wnd=new Window(instance_handle);
	CommandArgument cmd_arg(true);

	if(app_ptr->init()){
		app_ptr->run(cmd_arg,cmd_show);
		while(true){
			if(!(app_ptr->getMainWnd()->handleMessages())){
				break;
			}
		}
		app_ptr->cleanup();
	}

	int exit_code=app_ptr->getMainWnd()->getExitCode();

	SAFE_DELETE(app_ptr->m_main_wnd);
	SAFE_DELETE(app_ptr);

	return exit_code;
}
