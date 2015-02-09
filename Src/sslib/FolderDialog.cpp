//FolderDialog.cpp
//ファイル選択ダイアログでフォルダを選択

#include"../StdAfx.h"
#include"sslib.h"

#include<shlwapi.h>



namespace sslib{


//SHAutoComplete()
#ifndef SHACF_DEFAULT
	#define SHACF_DEFAULT					0x00000000
	#define SHACF_FILESYSTEM				0x00000001
	#define SHACF_URLALL					(SHACF_URLHISTORY|SHACF_URLMRU)
	#define SHACF_URLHISTORY				0x00000002
	#define SHACF_URLMRU					0x00000004
	#define SHACF_USETAB					0x00000008
	#define SHACF_FILESYS_ONLY				0x00000010
	#define SHACF_AUTOSUGGEST_FORCE_ON		0x10000000
	#define SHACF_AUTOSUGGEST_FORCE_OFF		0x20000000
	#define SHACF_AUTOAPPEND_FORCE_ON		0x40000000
	#define SHACF_AUTOAPPEND_FORCE_OFF		0x80000000
#endif


LRESULT CALLBACK OpenDialogProc(HWND wnd_handle,UINT msg,WPARAM wparam,LPARAM lparam){
	//インスタンスのポインタを取り出す
	FolderDialog* folder_dialog=reinterpret_cast<FolderDialog*>(::GetWindowLongPtr(wnd_handle,GWLP_USERDATA));

	if(msg==WM_COMMAND&&
	   HIWORD(wparam)==BN_CLICKED&&
	   LOWORD(wparam)==IDOK){
		if(folder_dialog){
			std::vector<TCHAR> dir_path(MAX_PATHW);

			::SendMessage(wnd_handle,CDM_GETFILEPATH,dir_path.size(),(LPARAM)&dir_path[0]);
			folder_dialog->m_dir_path.assign(&dir_path[0]);

			if(!folder_dialog->m_dir_path.empty()&&
			   path::fileExists(folder_dialog->m_dir_path.c_str())){
				if(path::isDirectory(folder_dialog->m_dir_path.c_str())){
					::EndDialog(wnd_handle,IDOK);
				}else{
					folder_dialog->m_dir_path=path::getParentDirectory(folder_dialog->m_dir_path);
					::EndDialog(wnd_handle,IDOK);
				}
			}
		}
		return 0;
	}
	return (folder_dialog)?::CallWindowProc(folder_dialog->m_open_proc,wnd_handle,msg,wparam,lparam):0;
}

//フックプロシージャ
UINT_PTR CALLBACK OpenHookProc(HWND wnd_handle,UINT message,WPARAM wparam,LPARAM lparam){
	if(message==WM_NOTIFY){
		FolderDialog* folder_dialog=reinterpret_cast<FolderDialog*>(((LPOFNOTIFY)lparam)->lpOFN->lCustData);
		LPNMHDR hdr=(LPNMHDR)lparam;
		HWND parent_handle=::GetParent(wnd_handle);

		if(hdr->code==CDN_INITDONE){
			std::vector<TCHAR> dir_path(MAX_PATHW);

			::SendMessage(parent_handle,
						  CDM_GETFILEPATH,
						  dir_path.size(),
						  (LPARAM)&dir_path[0]);
			folder_dialog->m_dir_path.assign(&dir_path[0]);

			::SendMessage(parent_handle,CDM_HIDECONTROL,stc2,0);
			::SendMessage(parent_handle,CDM_HIDECONTROL,cmb1,0);
			::SendMessage(parent_handle,CDM_HIDECONTROL,stc3,0);
//			::SendMessage(parent_handle,CDM_HIDECONTROL,cmb13,0);


			HWND cmb13_edit=::GetDlgItem(parent_handle,cmb13);

			while(1){
				HWND child=::GetWindow(cmb13_edit,GW_CHILD);
				if(child==NULL)break;
				else cmb13_edit=child;
			}
			//オートコンプリート機能無効化
			::SHAutoComplete(cmb13_edit,SHACF_AUTOAPPEND_FORCE_OFF|SHACF_AUTOSUGGEST_FORCE_OFF);


			::SendMessage(parent_handle,CDM_SETCONTROLTEXT,edt1,(LPARAM)path::getFileName(folder_dialog->m_dir_path).c_str());

			if(folder_dialog){
				//インスタンスのポインタを保存
				::SetWindowLongPtr(parent_handle,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(folder_dialog));

				folder_dialog->m_open_proc=(WNDPROC)SetWindowLongPtr(parent_handle,GWLP_WNDPROC,(LPARAM)OpenDialogProc);
			}

			return true;
		}else if(hdr->code==CDN_FILEOK){
			std::vector<TCHAR> dir_path(MAX_PATHW);

			::SendMessage(parent_handle,
						  CDM_GETFILEPATH,
						  dir_path.size(),
						  (LPARAM)&dir_path[0]);
			if(folder_dialog){
				folder_dialog->m_dir_path.assign(&dir_path[0]);
				if(!path::isDirectory(folder_dialog->m_dir_path.c_str())){
					folder_dialog->m_dir_path.assign(path::getParentDirectory(folder_dialog->m_dir_path));
				}
			}
		}else if(hdr->code==CDN_SELCHANGE){
			std::vector<TCHAR> dir_path(MAX_PATHW);

			::SendMessage(parent_handle,
						  CDM_GETFILEPATH,
						  dir_path.size(),
						  (LPARAM)&dir_path[0]);

			if(folder_dialog){
				folder_dialog->m_dir_path.assign(&dir_path[0]);
			}

			::SendMessage(parent_handle,
						  CDM_SETCONTROLTEXT,
						  edt1,
						  (LPARAM)path::getFileName(folder_dialog->m_dir_path).c_str());
		}else if(hdr->code==CDN_FOLDERCHANGE){
			::SendMessage(parent_handle,CDM_SETCONTROLTEXT,edt1,(LPARAM)_T(""));
		}
	}
	return false;
}

//「ファイルを開く」ダイアログ表示
bool FolderDialog::doModalOpen(tstring* file_path,HWND wnd_handle,const TCHAR* filter,const TCHAR* title,const TCHAR* init_dir){
	bool result=false;

	m_ofn_open.Flags|=OFN_NOVALIDATE|OFN_ENABLEHOOK|OFN_ENABLESIZING;

	m_ofn_open.lpfnHook=OpenHookProc;
	m_ofn_open.lCustData=reinterpret_cast<LPARAM>(this);

	if(m_template_id){
		m_ofn_open.Flags|=OFN_ENABLETEMPLATE;
		m_ofn_open.lpTemplateName=MAKEINTRESOURCE(m_template_id);
	}

	result=FileDialog::doModalOpen<std::list<tstring> >(NULL,wnd_handle,false,filter,title,init_dir);
	*file_path=m_dir_path;
	return result;
}

//namespace sslib
}
