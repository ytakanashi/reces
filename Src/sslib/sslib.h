//sslib.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//                  sslib ver.1.38
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _SSLIB_H_A4D33CA0_5DAF_4b2c_8C16_B956229BC4AD
#define _SSLIB_H_A4D33CA0_5DAF_4b2c_8C16_B956229BC4AD


#ifndef MAX_PATHW
	#define	MAX_PATHW 32768
#endif

//数値が範囲内にあるかどうか
template<typename T>T clamp(T value,T low,T high){
	return (value<low)?low:((value>high)?high:value);
}

//文字列がNULLでも空でもない
template<typename T>bool strvalid(T str){
	return str!=NULL&&*str!='\0';
}

//delete後NULL代入
#define SAFE_DELETE(v){delete (v);(v)=NULL;}
#define SAFE_DELETE_ARRAY(a){delete[] (a);(a)=NULL;}
//CloseHandle()後NULL代入
#define SAFE_CLOSE(h)if(h){CloseHandle(h);(h)=NULL;}
#define SAFE_CLOSE_EX(h,v)if(h){CloseHandle(h);(h)=(v);}
//配列の要素数
#define ARRAY_SIZEOF(array) (sizeof(array)/sizeof(array[0]))
//メンバの要素数
#define member_sizeof(type,member) ARRAY_SIZEOF(((type*)0)->member)


#define MAKEQWORD(high,low) (((long long)high<<32)|low)

#ifdef _DEBUG
	#define dprintf(...) _tprintf(__VA_ARGS__)
#else
	#define dprintf(...)
#endif

#include"Misc.h"

#include"FormatString.h"

#include"String.h"

#include"Path.h"

#include"Console.h"

#include"Dialog.h"

#include"CommandArgument.h"

#include"Window.h"

#ifndef SSLIB_GUI
	#ifndef SSLIB_GUI_DIALOG
		#include"ConsoleApp.h"
	#else
		#include"DialogApp.h"
	#endif
#else
	#include"WindowApp.h"
#endif
extern void createInstance();


#include"ProgressBar.h"

#include"File.h"

#include"Library.h"

#include"CfgFile.h"

#include"FileDialog.h"
#include"FolderDialog.h"

#include"FileSearch.h"

#include"FileOperation.h"

#include"EnvironmentVariable.h"

#include"ApiHook.h"


//GUI
#if defined(SSLIB_GUI)||defined(SSLIB_GUI_DIALOG)
#include"gui/DropFiles.h"
#include"gui/Control.h"
#include"gui/CtrlAHook.h"
#include"gui/Tab.h"
#include"gui/ListView.h"
#endif


//Ver.1.38(150209)
//reces Ver.0.00r25/gui4reces Ver.0.0.1.2で使用

//Ver.1.37(141223)
//reces Ver.0.00r24で使用

//Ver.1.36(141203)
//reces Ver.0.00r24aで使用

//Ver.1.35(141013)
//reces Ver.0.00r23/gui4reces Ver.0.0.1.1で使用

//Ver.1.34(140831)
//reces Ver.0.00r22/gui4reces Ver.0.0.1.0で使用

//Ver.1.33(140727)
//reces Ver.0.00r21/gui4reces Ver.0.0.0.9で使用

//Ver.1.32(140426)
//reces Ver.0.00r20で使用

//Ver.1.31(140325)
//reces Ver.0.00r19で使用

//Ver.1.30(140221)
//reces Ver.0.00r18で使用

//Ver.1.26(140208)
//reces Ver.0.00r17差し替え版で使用

//Ver.1.25(140207)
//reces Ver.0.00r17で使用

//Ver.1.20(140119)
//reces Ver.0.00r16で使用

//Ver.1.15(140103)
//reces Ver.0.00r15/gui4reces Ver.0.0.0.8で使用

//Ver.1.10(131208)
//reces Ver.0.00r14/gui4reces Ver.0.0.0.7で使用

//Ver.1.05(130710)
//reces Ver.0.00r13で使用

//ver.1.00(130619)
//reces Ver.0.00r12で使用


#endif //_SSLIB_H_A4D33CA0_5DAF_4b2c_8C16_B956229BC4AD
