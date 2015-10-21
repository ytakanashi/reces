//ConsoleApp.cpp
//CUIなアプリ専用汎用クラス

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{

bool ConsoleApp::m_terminated=false;

bool isTerminated(){
	return ConsoleApp::m_terminated;
}

void terminateApp(bool force){
	ConsoleApp::m_terminated=true;
	if(force)app()->ctrlC();
}

bool ConsoleApp::init(){
	return true;
}

void ConsoleApp::cleanup(){
}

void ConsoleApp::usage(){
}

void ConsoleApp::ctrlCEvent(){
	cleanup();
}

//namespace sslib
}

using namespace sslib;

//CUIなアプリのエントリポイント
int main(){
	ConsoleApp* app_ptr=app();

	{
		misc::CtrlCEventManager ctrlc_event_manager;

		ctrlc_event_manager.addCtrlCEvent(app_ptr);

		CommandArgument cmd_arg(CommandArgument::READ_STDIN|CommandArgument::DO_WILDCARD);

		if(app_ptr->init()){
			if(!app_ptr->run(cmd_arg)){
				if(app_ptr->getUsageFlag()){
					app_ptr->usage();
				}
				app_ptr->setExitCode(EXIT_FAILURE);
			}
			app_ptr->cleanup();
		}else{
			if(app_ptr->getUsageFlag()){
				app_ptr->usage();
			}
			app_ptr->setExitCode(EXIT_FAILURE);
		}
	}

	return app_ptr->getExitCode();
}
