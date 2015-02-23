//StdAfx.h
//共通ヘッダファイル

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r26 by x@rgs
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

#ifdef _DEBUG
	#define SOFTWARE_VERSION _T("0.00r26a_Debug")
#else
	#define SOFTWARE_VERSION _T("0.00r26a")
#endif

#include"sslib/sslib.h"

inline void msg(){MessageBox(NULL,NULL,NULL,MB_OK);}
inline void msg(const TCHAR* format,...){
	if(!format){msg();return;}
	va_list argp;
	va_start(argp,format);

	MessageBox(NULL,sslib::format(format,argp).c_str(),NULL,MB_OK);

	va_end(argp);
}

#endif //_STDAFX_H_B6023879_CF6F_45f4_A1D4_7DBAC5E430F2
