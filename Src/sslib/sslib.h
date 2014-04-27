//sslib.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//                  sslib ver.1.32
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _SSLIB_H_A4D33CA0_5DAF_4b2c_8C16_B956229BC4AD
#define _SSLIB_H_A4D33CA0_5DAF_4b2c_8C16_B956229BC4AD

#include"Misc.h"

#include"VariableArgument.h"

#include"String.h"

#include"Path.h"

#include"Console.h"

#include"Dialog.h"

#include"CommandArgument.h"

#include"Window.h"

#ifndef SSLIB_GUI
	#ifndef SSLIB_GUI_DIALOG
		#include"ConsoleApp.h"
		extern void createInstance();
	#else
		#include"DialogApp.h"
		extern void createInstance();
	#endif
#else
	#include"WindowApp.h"
	extern void createInstance();
#endif

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
