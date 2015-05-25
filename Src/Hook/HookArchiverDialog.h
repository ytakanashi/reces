//HookArchiverDialog.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r28 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _HOOKPASSWORDDIALOG_H_E9EB4B70_B490_45BE_BF59_CF333996E66E
#define _HOOKPASSWORDDIALOG_H_E9EB4B70_B490_45BE_BF59_CF333996E66E



#ifdef BUILD_DLL
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT //__declspec(dllimport)
#endif

const TCHAR EV_END_HOOK[]=_T("_EV_END_HOOK_");

enum HOOK_DIALOG_TYPE{
	HOOK_7ZIP32_PASSWORD=1,
	HOOK_7ZIP32_COMPRESS,
	HOOK_UNRAR32_PASSWORD,
	HOOK_7ZIP32_UNRAR32_EXTRACT,
	HOOK_XACRETT_PASSWORD,
	HOOK_XACRETT_EXTRACT
};

#ifdef __cplusplus
extern "C"{
#endif

//WH_CBTフックをインストール
bool DLL_EXPORT installPasswordDialogHook(const DWORD hook_process_id,const unsigned int thread_id,const unsigned int msg);
//フックをアンインストール
bool DLL_EXPORT uninstallPasswordDialogHook();

#ifdef __cplusplus
}
#endif

#endif //_HOOKPASSWORDDIALOG_H_E9EB4B70_B490_45BE_BF59_CF333996E66E
