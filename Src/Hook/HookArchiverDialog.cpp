//HookArchiverDialog.cpp
//パスワード入力ダイアログなどをフック

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"../StdAfx.h"
#include"HookArchiverDialog.h"


UINT WM_HOOKDIALOG;
HHOOK g_hook_handle;
HINSTANCE g_instance_handle;
DWORD g_hook_process_id;
unsigned int g_post_thread_id;

struct ENUM_CHILD_PARAM{
	int dialog_type;
	bool* result;

	ENUM_CHILD_PARAM(int dialog_type_,bool* result_){
		dialog_type=dialog_type_;
		result=result_;
	}
};


//ダイアログを画面外に追いやる
void hideDialog(HWND wnd_handle){
	SetWindowLongPtr(wnd_handle,GWL_EXSTYLE,GetWindowLongPtr(wnd_handle,GWL_EXSTYLE)|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE);
	SetWindowPos(wnd_handle,HWND_BOTTOM,-1000,0,0,0,SWP_NOSENDCHANGING);
}

BOOL CALLBACK EnumChildProc(HWND wnd_handle,LPARAM lparam){
	WINDOWINFO info;

	GetWindowInfo(wnd_handle,&info);

	if(info.dwStyle&ES_PASSWORD){
		int dialog_type=((ENUM_CHILD_PARAM*)lparam)->dialog_type;
		HWND parent_handle=GetParent(wnd_handle);

		//ダイアログを画面外に追いやる
		hideDialog(parent_handle);

		PostThreadMessage(g_post_thread_id,WM_HOOKDIALOG,(WPARAM)wnd_handle,(LPARAM)dialog_type);
		*((ENUM_CHILD_PARAM*)lparam)->result=true;

		return FALSE;
	}
	return TRUE;
}

//WH_CBTのコールバック
LRESULT CALLBACK CBTProc(int code,WPARAM wparam,LPARAM lparam){
	if(g_hook_process_id==GetCurrentProcessId()&&
	   code==HCBT_ACTIVATE){
		TCHAR window_name[1024];
		TCHAR class_name[255];
		HWND wnd_handle=(HWND)wparam;
		int hook_dialog=0;

		GetWindowText(wnd_handle,window_name,ARRAY_SIZEOF(window_name));
		GetClassName(wnd_handle,class_name,ARRAY_SIZEOF(class_name));

		if(lstrcmp(class_name,_T("#32770"))==0){
			if(lstrcmp(window_name,_T("ファイルの暗号化"))==0||
			   lstrcmp(window_name,_T("ファイルの復号化"))==0){
				//7-zip32.dll
				//パスワードダイアログ
				hook_dialog=HOOK_7ZIP32_PASSWORD;
			}else if(lstrcmp(window_name,_T("書庫は暗号化されています"))==0){
				//unrar32.dll
				//パスワードダイアログ
				hook_dialog=HOOK_UNRAR32_PASSWORD;
			}
#ifndef _WIN64
			 else if(lstrcmp(window_name,_T("Please Input Password"))==0){
				//XacRett.dll
				//パスワードダイアログ
				hook_dialog=HOOK_XACRETT_PASSWORD;
			}else{
				static TCHAR xacrett_title[]=_T("XacRett: ");
				window_name[ARRAY_SIZEOF(xacrett_title)-1]='\0';
				if(lstrcmp(window_name,xacrett_title)==0){
					//XacRett.dll
					//解凍ダイアログ
					hook_dialog=HOOK_XACRETT_EXTRACT;

					hideDialog(wnd_handle);

					//ダイアログのハンドルを渡す
					PostThreadMessage(g_post_thread_id,WM_HOOKDIALOG,(WPARAM)wnd_handle,(LPARAM)hook_dialog);
					return true;
				}
			}
#endif

			if(hook_dialog){
				bool result=false;
				ENUM_CHILD_PARAM enum_child_param(hook_dialog,&result);

				EnumChildWindows(wnd_handle,EnumChildProc,(LPARAM)&enum_child_param);
				return result;
			}
		}
	}

	return CallNextHookEx(NULL/*This parameter is ignored.*/,code,wparam,lparam);
}

bool DLL_EXPORT installPasswordDialogHook(const DWORD hook_process_id,const unsigned int post_thread_id,const unsigned int msg){
	if(post_thread_id==0||hook_process_id==0)return false;

	if((g_hook_handle=SetWindowsHookEx(WH_CBT,CBTProc,g_instance_handle,0))!=NULL){
		g_hook_process_id=hook_process_id;
		g_post_thread_id=post_thread_id;
		WM_HOOKDIALOG=msg;
		return true;
	}
	return false;
}

//フックをアンインストール
bool DLL_EXPORT uninstallPasswordDialogHook(){
	bool result=false;

	if(g_hook_handle!=NULL){
		result=UnhookWindowsHookEx(g_hook_handle)!=0;
		SendMessageTimeout(HWND_BROADCAST,WM_NULL,0,0,SMTO_ABORTIFHUNG,100,NULL);
		g_hook_handle=NULL;
	}

	return result;
}

//extern "C"必須
extern "C" BOOL WINAPI DllMain(HINSTANCE instance_handle,DWORD reason,LPVOID reserved){
	switch(reason){
		case DLL_PROCESS_ATTACH:
			// attach to process
			// return FALSE to fail DLL load
			//Dllのハンドルを保存
			g_instance_handle=instance_handle;
			//DLL_THREAD_ATTACHとDLL_THREAD_DETACHを無効にする
			DisableThreadLibraryCalls(instance_handle);
			break;

		case DLL_PROCESS_DETACH:
			// detach from process
			break;

		case DLL_THREAD_ATTACH:
			// attach to thread
			break;

		case DLL_THREAD_DETACH:
			// detach from thread
			break;
	}
	return TRUE;// succesful
}
