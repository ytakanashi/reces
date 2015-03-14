//FileDialog.h

#ifndef _FILEDIALOG_H_46D6CD6B_A37F_45f3_A38B_B6AD40254A61
#define _FILEDIALOG_H_46D6CD6B_A37F_45f3_A38B_B6AD40254A61



namespace sslib{
class FileDialog{
public:
	FileDialog():m_ofn_open(),m_ofn_save(){}
	virtual ~FileDialog(){}

protected:
	OPENFILENAME m_ofn_open;
	OPENFILENAME m_ofn_save;

public:
	//「ファイルを開く」ダイアログ表示[複数選択可能]
	bool doModalOpen(std::list<tstring>* file_list,HWND wnd_handle=NULL,bool multi_select=false,const TCHAR* filter=NULL,const TCHAR* title=NULL,const TCHAR* init_dir=NULL,const TCHAR* init_name=NULL);
	//「ファイルを開く」ダイアログ表示
	bool doModalOpen(tstring* file_path,HWND wnd_handle=NULL,const TCHAR* filter=NULL,const TCHAR* title=NULL,const TCHAR* init_dir=NULL,const TCHAR* init_name=NULL);
	//「ファイルを保存」ダイアログ表示
	bool doModalSave(tstring* file_path,HWND wnd_handle=NULL,const TCHAR* filter=NULL,const TCHAR* title=NULL,const TCHAR* init_dir=NULL,const TCHAR* init_name=NULL);
};

//namespace sslib
}

#endif //_FILEDIALOG_H_46D6CD6B_A37F_45f3_A38B_B6AD40254A61
