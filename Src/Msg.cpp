//Msg.cpp
//情報の表示

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r27 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"Msg.h"
#include"ArcCfg.h"


using namespace sslib;
namespace msg{

bool info(const TCHAR* msg,...){
	if(CFG.no_display.no_information)return false;

	DWORD written_chars=0;

	va_list argp;
	va_start(argp,msg);

	STDOUT.write(msg,argp,&written_chars);

	va_end(argp);
	return true;
}

void err(const TCHAR* msg,...){
	if(!strvalid(msg)||
	   CFG.no_display.no_errmsg)return;

	Console std_err_handle(STD_ERROR_HANDLE);
	DWORD written_chars=0;

	//文字色の変更
	std_err_handle.setFGColor(Console::HIGH_RED);

	va_list argp;
	va_start(argp,msg);

	std_err_handle.outputString(_T("error: "));
	std_err_handle.write(msg,argp,&written_chars);

	va_end(argp);

	//文字色を元に戻す
	std_err_handle.resetColors();
	return;
}

void lasterr(){
	void* msg_buffer=NULL;

	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
					FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					::GetLastError(),
					MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT),
					static_cast<TCHAR*>(msg_buffer),
					0,
					NULL
					);
	if(msg_buffer!=NULL)err(_T("%s\n"),static_cast<TCHAR*>(msg_buffer));
	::LocalFree(msg_buffer);
	return;
}

//namespace msg
}
