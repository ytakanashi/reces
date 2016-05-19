//FileDialog.cpp
//ファイル選択ダイアログ

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{


//「ファイルを開く」ダイアログ表示[複数選択可能]
bool FileDialog::doModalOpen(std::list<tstring>* file_list,HWND wnd_handle,bool multi_select,const TCHAR* filter,const TCHAR* title,const TCHAR* init_dir,const TCHAR* init_name){
	bool result=false;
	std::vector<TCHAR> file_buffer(50000);

	if(init_name!=NULL){
		lstrcpyn(&file_buffer[0],init_name,file_buffer.size()-2);
		lstrcat(&file_buffer[0],'\0');
	}

	m_ofn_open.lStructSize=sizeof(OPENFILENAME);
	m_ofn_open.hwndOwner=wnd_handle;
	m_ofn_open.hInstance=::GetModuleHandle(NULL);
	m_ofn_open.Flags|=OFN_EXPLORER|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR;
	if(multi_select){
		m_ofn_open.Flags|=OFN_ALLOWMULTISELECT;
	}

	if(filter!=NULL){
		m_ofn_open.lpstrFilter=filter;
	}else{
		m_ofn_open.lpstrFilter=_T("All Files (*.*)\0*.*\0\0");
	}

	m_ofn_open.lpstrInitialDir=init_dir;
	m_ofn_open.lpstrTitle=title;
	m_ofn_open.lpstrFile=&file_buffer[0];
	m_ofn_open.nMaxFile=file_buffer.size();

	if(::GetOpenFileName(&m_ofn_open)){
		if(!path::isDirectory(&file_buffer[0])){
			if(file_list!=NULL)file_list->push_back(&file_buffer[0]);
			result=true;
		}else{
			//複数選択
			tstring parent_dir(&file_buffer[0]);

			for(int i=parent_dir.length()+1;;){
				tstring file(&file_buffer[i]);

				if(file.length()==0)break;

				//dir+file_name
				if(file_list!=NULL)file_list->push_back(path::addTailSlash(parent_dir)+=file);

				//次のファイルへ
				i+=file.length()+1;
			}

			result=true;
		}
	}else{
		DWORD dlg_err=::CommDlgExtendedError();

		if(dlg_err!=0){
			if(dlg_err==FNERR_BUFFERTOOSMALL){
				//FNERR_BUFFERTOOSMALL[バッファが小さい]
				::MessageBox(wnd_handle,_T("選択したファイルの数が多過ぎます。"),NULL,MB_ICONWARNING);
			}else{
				::MessageBox(wnd_handle,format(_T("ファイルの選択に失敗しました。\nErrCode:0x%08X"),dlg_err).c_str(),NULL,MB_ICONWARNING);
			}
		}
	}
	return result;
}

//「ファイルを開く」ダイアログ表示
bool FileDialog::doModalOpen(tstring* file_path,HWND wnd_handle,const TCHAR* filter,const TCHAR* title,const TCHAR* init_dir,const TCHAR* init_name){
	bool result=false;

	std::list<tstring> file_list;

	result=doModalOpen(&file_list,wnd_handle,false,filter,title,init_dir);
	if(result&&file_path!=NULL)file_path->assign(*file_list.begin());
	return result;
}

//「ファイルを保存」ダイアログ表示
bool FileDialog::doModalSave(tstring* file_path,HWND wnd_handle,const TCHAR* filter,const TCHAR* title,const TCHAR* init_dir,const TCHAR* init_name,const TCHAR* default_ext){
	bool result=false;
	std::vector<TCHAR> file_buffer(MAX_PATHW);

	if(init_name!=NULL){
		lstrcpyn(&file_buffer[0],init_name,file_buffer.size()-2);
		lstrcat(&file_buffer[0],'\0');
	}

	m_ofn_save.lStructSize=sizeof(OPENFILENAME);
	m_ofn_save.hwndOwner=wnd_handle;
	m_ofn_save.Flags|=OFN_EXPLORER|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT;

	if(filter!=NULL){
		m_ofn_save.lpstrFilter=filter;
	}else{
		m_ofn_save.lpstrFilter=_T("All Files (*.*)\0*.*\0\0");
	}

	m_ofn_save.lpstrInitialDir=init_dir;
	m_ofn_save.lpstrTitle=title;
	m_ofn_save.lpstrFile=&file_buffer[0];
	m_ofn_save.nMaxFile=file_buffer.size();
	m_ofn_save.lpstrDefExt=default_ext;

	if(!::GetSaveFileName(&m_ofn_save)){
		DWORD dlg_err=::CommDlgExtendedError();

		if(dlg_err!=0){
			::MessageBox(wnd_handle,format(_T("ファイルの選択に失敗しました。\nErrCode:0x%08X"),dlg_err).c_str(),NULL,MB_ICONWARNING);
		}
	}else{
		if(file_path!=NULL)file_path->assign(&file_buffer[0]);
		result=true;
	}
	return result;
}

//namespace sslib
}
