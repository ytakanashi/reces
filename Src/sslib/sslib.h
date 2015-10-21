//sslib.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//                  sslib ver.1.46
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _SSLIB_H_A4D33CA0_5DAF_4b2c_8C16_B956229BC4AD
#define _SSLIB_H_A4D33CA0_5DAF_4b2c_8C16_B956229BC4AD


#ifdef __cplusplus

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

// __cplusplus
#endif

#ifndef MAX_PATHW
	#define	MAX_PATHW 32768
#endif

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


#ifdef __cplusplus

#ifndef DISABLE_MISC
	#include"Misc.h"
#endif

#ifndef DISABLE_FORMAT_STRING
	#include"FormatString.h"
#endif

#ifndef DISABLE_STRING
	#include"String.h"
#endif

#ifndef DISABLE_STRING_EX
	#include"StringEx.h"
#endif

#ifndef DISABLE_PATH
	#include"Path.h"
#endif

#ifndef DISABLE_CONSLE
	#include"Console.h"
#endif

#ifndef DISABLE_DIALOG
	#include"Dialog.h"
#endif

#ifndef DISABLE_COMMAND_ARGUMENT
	#include"CommandArgument.h"
#endif

#ifndef DISABLE_WINDOW
	#include"Window.h"
#endif

#ifndef SSLIB_GUI
	#ifndef SSLIB_GUI_DIALOG
		#include"ConsoleApp.h"

#define MAIN_INSTANCE(class_name)\
	namespace{class_name _main_app_;}\
	inline ConsoleApp* app(){\
		return &_main_app_;\
	}\

	#else
		#include"DialogApp.h"
#define MAIN_INSTANCE(class_name)\
	namespace{class_name _main_app_;}\
	inline DialogApp* app(){\
		return &_main_app_;\
	}\

	#endif
#else
	#include"WindowApp.h"
#define MAIN_INSTANCE(class_name)\
	namespace{class_name _main_app_;}\
	inline WindowApp* app(){\
		return &_main_app_;\
	}\

#endif

#if !defined(SSLIB_GUI)&&!defined(SSLIB_GUI_DIALOG)
	#ifdef _MSC_VER
		#define IS_TERMINATED isTerminated()
	#endif
#else
	#define IS_TERMINATED false
#endif

#ifndef DISABLE_PROGRESS_BAR
	#include"ProgressBar.h"
#endif

#ifndef DISABLE_FILE
	#include"File.h"
#endif

#ifndef DISABLE_LIBRARY
	#include"Library.h"
#endif

#ifndef DISABLE_CFG_FILE
	#include"CfgFile.h"
#endif

#ifndef DISABLE_FILE_DIALOG
	#include"FileDialog.h"
#endif
#ifndef DISABLE_FOLDER_DIALOG
	#include"FolderDialog.h"
#endif

#ifndef DISABLE_FILE_SEARCH
	#include"FileSearch.h"
#endif

#ifndef DISABLE_FILE_OPERATION
	#include"FileOperation.h"
#endif

#ifndef DISABLE_SPLIT_FILE
	#include"SplitFile.h"
#endif

#ifndef DISABLE_TEMP_FILE
	#include"TempFile.h"
#endif

#ifndef DISABLE_ENVIRONMENTAL_VARIABLE
	#include"EnvironmentVariable.h"
#endif

#ifndef DISABLE_API_HOOK
	#include"ApiHook.h"
#endif


//GUI
#if defined(SSLIB_GUI)||defined(SSLIB_GUI_DIALOG)
#ifndef DISABLE_COMMON_CONTROLS_EX
	#include"gui/CommonControlsEx.h"
#endif
#ifndef DISABLE_DROPFILES
	#include"gui/DropFiles.h"
#endif
#ifndef DISABLE_CONTROL
	#include"gui/Control.h"
#endif
#ifndef DISABLE_CTRL_A_HOOK
	#include"gui/CtrlAHook.h"
#endif
#ifndef DISABLE_MENU
	#include"gui/Menu.h"
#endif
#ifndef DISABLE_TAB
	#include"gui/Tab.h"
#endif
#ifndef DISABLE_LIST_VIEW
	#include"gui/ListView.h"
#endif
// defined(SSLIB_GUI)||defined(SSLIB_GUI_DIALOG)
#endif
// __cplusplus
#endif


//Ver.1.46
//reces Ver.0.00r30/gui4reces Ver.0.0.1.6で使用

//Ver.1.45
//TimeStamp Keeper Ver.1.01で使用

//Ver.1.44
//reces Ver.0.00r30aで使用

//Ver.1.43
//reces Ver.0.00r29/gui4reces Ver.0.0.1.5で使用

//Ver.1.42(150515)
//reces Ver.0.00r27/gui4reces Ver.0.0.1.4で使用

//Ver.1.41(150421)
//reces Ver.0.00r27aで使用

//Ver.1.40(150320)
//TimeStamp Keeper Ver.1.00で使用

//Ver.1.39(150314)
//reces Ver.0.00r26/gui4reces Ver.0.0.1.3で使用

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
//reces Ver.0.00r17差し替え版/u2s Ver.1.00で使用

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
