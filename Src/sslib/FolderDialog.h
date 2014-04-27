//FolderDialog.h

#ifndef _FOLDERDIALOG_H_47B8D935_C0B0_407c_AF6C_99DC4CB3A3B3
#define _FOLDERDIALOG_H_47B8D935_C0B0_407c_AF6C_99DC4CB3A3B3



namespace sslib{
class FolderDialog:public FileDialog{
public:
	FolderDialog():
		m_open_proc(NULL),
		m_dir_path(),
		m_template_id(0){}
	FolderDialog(WORD template_id):
		m_open_proc(NULL),
		m_dir_path(),
		m_template_id(template_id){}
	virtual ~FolderDialog(){}


public:
	WNDPROC m_open_proc;
	tstring m_dir_path;
	WORD m_template_id;

public:
	//「ファイルを開く」でフォルダを選択できるダイアログ表示
	bool doModalOpen(tstring* file_path,HWND wnd_handle=NULL,const TCHAR* filter=NULL,const TCHAR* title=NULL,const TCHAR* init_dir=NULL);
};

//namespace sslib
}

#endif //_FOLDERALOG_H_47B8D935_C0B0_407c_AF6C_99DC4CB3A3B3
