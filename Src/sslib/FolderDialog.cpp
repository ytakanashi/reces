//FolderDialog.cpp
//ファイル選択ダイアログでフォルダを選択

#include"../StdAfx.h"
#include"sslib.h"

#include<shlwapi.h>
#include<shlobj.h>



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

#ifndef BFFM_INITIALIZED
	#define BFFM_INITIALIZED 1
	#define BFFM_SELCHANGED 2
	#define BFFM_VALIDATEFAILEDA 3
	#define BFFM_VALIDATEFAILEDW 4
	#define BFFM_SETSTATUSTEXTA (WM_USER + 100)
	#define BFFM_SETSTATUSTEXTW (WM_USER + 104)
	#define BFFM_ENABLEOK (WM_USER + 101)
	#define BFFM_SETSELECTIONA (WM_USER + 102)
	#define BFFM_SETSELECTIONW (WM_USER + 103)
	#define BFFM_SETOKTEXT (WM_USER + 105)
	#define BFFM_SETEXPANDED (WM_USER + 106)

	#ifdef UNICODE
		#define BFFM_SETSTATUSTEXT  BFFM_SETSTATUSTEXTW
		#define BFFM_SETSELECTION   BFFM_SETSELECTIONW
		#define BFFM_VALIDATEFAILED BFFM_VALIDATEFAILEDW
	#else
		#define BFFM_SETSTATUSTEXT  BFFM_SETSTATUSTEXTA
		#define BFFM_SETSELECTION   BFFM_SETSELECTIONA
		#define BFFM_VALIDATEFAILED BFFM_VALIDATEFAILEDA
	#endif
#endif


BOOL CALLBACK EnumChildCallbackProc(HWND wnd_handle,LPARAM lparam){
	TCHAR class_name[MAX_PATH]={};

	::GetClassName(wnd_handle,class_name,ARRAY_SIZEOF(class_name));
	if(lstrcmpi(class_name,WC_TREEVIEWW)==0){
		if(lparam){
			*(HWND*)lparam=wnd_handle;
		}
		return FALSE;
	}
	return TRUE;
}

int CALLBACK BrowseForFolderCallbackProc(HWND wnd_handle,UINT message,LPARAM lparam,LPARAM data){
	static HWND edit_handle,tree_handle;

	switch(message){
		case BFFM_INITIALIZED:{
			if(data){
				//初期ディレクトリ設定
				SendMessage(wnd_handle,BFFM_SETSELECTION,(WPARAM)true,(LPARAM)data);
				edit_handle=::FindWindowEx(wnd_handle,NULL,WC_EDIT,NULL);

				if(edit_handle){
					//エディットコントロールにオートコンプリート機能を実装
					::SHAutoComplete(edit_handle,SHACF_FILESYSTEM|SHACF_URLALL|SHACF_FILESYS_ONLY|SHACF_USETAB);
				}

				::EnumChildWindows(wnd_handle,EnumChildCallbackProc,(LPARAM)&tree_handle);
				::SetFocus(tree_handle);
			}
			break;
		}

		case BFFM_SELCHANGED:{
			LPITEMIDLIST item_id_list=(LPITEMIDLIST)lparam;
			TCHAR directory[MAX_PATHW]={};

			//TODO:マイコンピュータを素通りしてしまう
			if(SHGetPathFromIDList(item_id_list,directory)){
				if(edit_handle){
					SendMessage(edit_handle,WM_SETTEXT,(WPARAM)0,(LPARAM)path::addTailSlash(directory).c_str());
				}
			}
			TreeView_EnsureVisible(tree_handle,TreeView_GetSelection(tree_handle));
			::SetFocus(tree_handle);
			break;
		}

		//無効なディレクトリ名であった場合
		case BFFM_VALIDATEFAILED:
			return 1;

		default:
			break;
	}
	return 0;
}

bool FolderDialog::doModalOpen(tstring* file_path,HWND wnd_handle,const TCHAR* title,const TCHAR* init_dir){
	bool result=false;
	std::vector<TCHAR> file_buffer(MAX_PATHW);

	::CoInitialize(NULL);
	LPTSTR buffer=NULL;
	LPITEMIDLIST id_root=NULL;
	LPITEMIDLIST item_id_list=NULL;
	LPMALLOC lpmalloc=NULL;
	if(FAILED(::SHGetMalloc(&lpmalloc)))return false;
	if((buffer=(LPTSTR)lpmalloc->Alloc(MAX_PATHW))==NULL)return false;
	if(!SUCCEEDED(::SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP,&id_root))){
		lpmalloc->Free(buffer);
		::CoUninitialize();
		return false;
	}

	//デスクトップのパスを取得
	SHFILEINFO shFileInfo={};

	::SHGetFileInfo((LPCTSTR)id_root,0,&shFileInfo,sizeof(SHFILEINFO),SHGFI_DISPLAYNAME|SHGFI_PIDL);

	BROWSEINFO browse_info={};
	browse_info.hwndOwner=wnd_handle;
	browse_info.pidlRoot=id_root;
	browse_info.pszDisplayName=buffer;
	browse_info.lpszTitle=(title)?title:_T("フォルダを選択してください");
	//BIF_DONTGOBELOWDOMAINを有効にすると、デスクトップ上のファイルが列挙されてしまう
	browse_info.ulFlags=BIF_USENEWUI|BIF_NONEWFOLDERBUTTON|BIF_RETURNONLYFSDIRS/*|BIF_DONTGOBELOWDOMAIN*/|BIF_VALIDATE;
	browse_info.lpfn=BrowseForFolderCallbackProc;
	browse_info.lParam=reinterpret_cast<LPARAM>((init_dir)?init_dir:shFileInfo.szDisplayName);

	item_id_list=::SHBrowseForFolder(&browse_info);
	if(item_id_list){
		if(::SHGetPathFromIDList(item_id_list,&file_buffer[0])){
			if(file_path!=NULL){
				file_path->assign(&file_buffer[0]);
				result=true;
			}
		}
		lpmalloc->Free(item_id_list);
	}
	lpmalloc->Free(id_root);
	lpmalloc->Free(buffer);
	lpmalloc->Release();
	::CoUninitialize();
	return result;
}

#if 0
LRESULT CALLBACK OpenDialogProc(HWND wnd_handle,UINT msg,WPARAM wparam,LPARAM lparam){
	//インスタンスのポインタを取り出す
	FolderDialog* folder_dialog=reinterpret_cast<FolderDialog*>(::GetWindowLongPtr(wnd_handle,GWLP_USERDATA));

	if(folder_dialog&&
	   msg==WM_COMMAND&&
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

	result=FileDialog::doModalOpen(NULL,wnd_handle,false,filter,title,init_dir);
	*file_path=m_dir_path;
	return result;
}
#endif

//namespace sslib
}
