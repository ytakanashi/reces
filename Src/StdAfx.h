//StdAfx.h
//共通ヘッダファイル

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r27 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _STDAFX_H_B6023879_CF6F_45f4_A1D4_7DBAC5E430F2
#define _STDAFX_H_B6023879_CF6F_45f4_A1D4_7DBAC5E430F2

#ifndef STRICT
	#define STRICT
#endif
#ifndef UNICODE
	#define UNICODE
#endif
#ifndef _UNICODE
	#define _UNICODE
#endif

#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0400
#define WINVER 0x0500

#ifdef _DEBUG
	#define _HAS_ITERATOR_DEBUGGING 0
#endif

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<assert.h>

#include<process.h>

#include<windows.h>
#include<tchar.h>

#include<string>
#include<list>
#include<vector>
#include<algorithm>

typedef std::basic_string<TCHAR>tstring;

#ifndef _tcstold
	#ifdef _UNICODE
		#define _tcstold wcstold
	#else
		#define _tcstold strtold
	#endif //_UNICODE
#endif //_tcstold

#ifndef _tcstoll
	#ifdef _UNICODE
		#ifdef _MSC_VER
			#define _tcstoll _wcstoi64
		#else
			#define _tcstoll wcstoll
		#endif
	#else
		#ifdef _MSC_VER
			#define _tcstoll _strtoi64
		#else
			#define _tcstoll strtoll
		#endif
	#endif //_UNICODE
#endif //_tcstoll

#define INNER_FUNC(name,v)\
	static struct{\
		v\
	}name;\

#ifdef _DEBUG
	#define SOFTWARE_VERSION _T("0.00r27a_Debug")
#else
	#define SOFTWARE_VERSION _T("0.00r27a")
#endif

//#define DISABLE_MISC
//#define DISABLE_FORMAT_STRING
//#define DISABLE_STRING
//#define DISABLE_STRING_EX
//#define DISABLE_PATH
//#define DISABLE_CONSOLE
#define DISABLE_DIALOG
//#define DISABLE_COMMAND_ARGUMENT
#define DISABLE_WINDOW
//#define DISABLE_PROGRESS_BAR
//#define DISABLE_FILE
//#define DISABLE_LIBRARY
//#define DISABLE_CFG_FILE
//#define DISABLE_FILE_SEARCH
//#define DISABLE_FILE_OPERATION
//#define DISABLE_SPLIT_FILE
//#define DISABLE_TEMP_FILE
//#define DISABLE_ENVIRONMENTAL_VARIABLE
//#define DISABLE_API_HOOK
#define DISABLE_COMMON_CONTROLS_EX
#define DISABLE_DROPFILES
#define DISABLE_CONTROL
#define DISABLE_CTRL_A_HOOK
#define DISABLE_MENU
#define DISABLE_TAB
#define DISABLE_LIST_VIEW

//#define SSLIB_GUI
//#define SSLIB_GUI_DIALOG
#include"sslib/sslib.h"

inline void dmsg(){MessageBox(NULL,NULL,NULL,MB_OK);}
inline void dmsg(const TCHAR* format,...){
	if(!format){dmsg();return;}
	va_list argp;
	va_start(argp,format);

	MessageBox(NULL,sslib::format(format,argp).c_str(),NULL,MB_OK);

	va_end(argp);
}

#endif //_STDAFX_H_B6023879_CF6F_45f4_A1D4_7DBAC5E430F2
