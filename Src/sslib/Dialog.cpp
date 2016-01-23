//Dialog.cpp
//ダイアログウクラス


#include"../StdAfx.h"
#include"sslib.h"


namespace sslib{


INT_PTR CALLBACK messageRouter(HWND dlg_handle,UINT message,WPARAM wparam,LPARAM lparam){
	Dialog* dlg=NULL;

	if(message==WM_INITDIALOG){
		dlg=reinterpret_cast<Dialog*>(lparam);
	}else{
		//インスタンスのポインタを取り出す
		dlg=reinterpret_cast<Dialog*>(::GetWindowLongPtr(dlg_handle,DWLP_USER));
	}
	return (dlg!=NULL)?dlg->handleMessage(dlg_handle,message,wparam,lparam):FALSE;
}

//モーダルダイアログを表示
INT_PTR Dialog::doModal(HWND parent_handle){
	setDlgType(MODAL);
	return ::DialogBoxParam(inst(),MAKEINTRESOURCE(id()),parent_handle,messageRouter,reinterpret_cast<LPARAM>(this));
}

//モードレスダイアログを表示
HWND Dialog::doModeless(HWND parent_handle,int cmd_show){
	setDlgType(MODELESS);
	HWND dlg_handle=::CreateDialogParam(inst(),MAKEINTRESOURCE(id()),parent_handle,messageRouter,reinterpret_cast<LPARAM>(this));
	showDialog(cmd_show);

	return dlg_handle;
}

//ダイアログを表示
bool Dialog::showDialog(int cmd_show){
	bool result=::ShowWindow(handle(),cmd_show)!=0;
//	::UpdateWindow(handle());
	return result;
}

//ダイアログを閉じる
bool Dialog::endDialog(UINT exit_code){
	setExitCode(exit_code);
	return ::EndDialog(handle(),exit_code)!=0;
}

INT_PTR Dialog::onInitDialog(WPARAM wparam,LPARAM lparam){
	return true;
}

INT_PTR Dialog::onClose(){
	if(type()==MODAL){
		::EndDialog(handle(),IDOK);
	}else{
		::DestroyWindow(handle());
	}
	return true;
}

INT_PTR Dialog::onDestroy(){
	if(type()==MODELESS)::PostQuitMessage(0);
	return true;
}

INT_PTR Dialog::onOk(WPARAM wparam,LPARAM lparam){
	return true;
}

INT_PTR Dialog::onCancel(WPARAM wparam,LPARAM lparam){
	return true;
}

INT_PTR Dialog::onCommand(WPARAM wparam,LPARAM lparam){
	return false;
}

INT_PTR Dialog::onSysCommand(WPARAM wparam,LPARAM lparam){
	return false;
}

INT_PTR Dialog::onNotify(WPARAM wparam,LPARAM lparam){
	return false;
}

INT_PTR Dialog::onPaint(){
	return false;
}

INT_PTR Dialog::onSize(WPARAM wparam,LPARAM lparam){
	return false;
}

INT_PTR Dialog::onMove(WPARAM wparam,LPARAM lparam){
	return false;
}

INT_PTR Dialog::onCtlColorStatic(WPARAM wparam,LPARAM lparam){
	//テキストの背景色を透過させる
	HDC dc=reinterpret_cast<HDC>(wparam);
	::SetBkMode(dc,TRANSPARENT);
	return true;
}

INT_PTR Dialog::onDropFiles(HDROP drop_handle){
	return false;
}

INT_PTR Dialog::onMessage(UINT message,WPARAM wparam,LPARAM lparam){
	return false;
}

LRESULT Dialog::sendMessage(UINT message,WPARAM wparam,LPARAM lparam){
	return ::SendMessage(handle(),message,wparam,lparam);
}

LRESULT Dialog::sendItemMessage(int item,UINT message,WPARAM wparam,LPARAM lparam){
	return ::SendDlgItemMessage(handle(),item,message,wparam,lparam);
}

bool Dialog::postMessage(UINT message,WPARAM wparam,LPARAM lparam){
	return ::PostMessage(handle(),message,wparam,lparam)!=0;
}

INT_PTR Dialog::handleMessage(HWND dlg_handle,UINT message,WPARAM wparam,LPARAM lparam){
	switch(message){
		case WM_INITDIALOG:{
			setHandle(dlg_handle);
			//lparamにインスタンスのポインタが代入されているので保存
			::SetWindowLongPtr(dlg_handle,DWLP_USER,lparam);
			return onInitDialog(wparam,lparam);
		}
		case WM_CLOSE:return onClose();
		case WM_DESTROY:return onDestroy();
		case WM_QUIT:{
			::SetWindowLongPtr(handle(),DWLP_USER,0);
			setHandle(NULL);
			return true;
		}
		case WM_COMMAND:{
			switch(LOWORD(wparam)){
				case IDOK:
					if(onOk(wparam,lparam)){endDialog(IDOK);return true;}
					else return false;
				case IDCANCEL:
					if(onCancel(wparam,lparam)){endDialog(IDCANCEL);return true;}
					else return false;
			}
			return onCommand(wparam,lparam);
		}
		case WM_SYSCOMMAND:return onSysCommand(wparam,lparam);
		case WM_NOTIFY:return onNotify(wparam,lparam);
		case WM_PAINT:return onPaint();
		case WM_SIZE:return onSize(wparam,lparam);
		case WM_MOVE:return onMove(wparam,lparam);
		case WM_DROPFILES:return onDropFiles(reinterpret_cast<HDROP>(wparam));
		default:return onMessage(message,wparam,lparam);
	}
}

//namespace sslib
}
