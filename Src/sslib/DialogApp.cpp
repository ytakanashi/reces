﻿//DialogApp.cpp
//GUIなダイアログアプリ専用汎用クラス

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{

bool DialogApp::init(){
	return true;
}

void DialogApp::cleanup(){
}

//namespace sslib
}

using namespace sslib;

//GUIなダイアログアプリのエントリポイント
int WINAPI WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show){
	DialogApp* app_ptr=app();
	CommandArgument cmd_arg(true);
	int exit_code=0;

	app_ptr->m_instance_handle=instance_handle;

	if(app_ptr->init()){
		exit_code=app_ptr->run(cmd_arg,cmd_show);
		app_ptr->cleanup();
	}

	return exit_code;
}
