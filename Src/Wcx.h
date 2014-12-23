//Wcx.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r24 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _WCX_H_1948985C_769E_421b_957B_71F189EAA154
#define _WCX_H_1948985C_769E_421b_957B_71F189EAA154


#include"WcxBase.h"
#include"PrivateProfile.h"
#include"FileInfo.h"


class Wcx:public WcxBase{
public:
	Wcx(const TCHAR* library_name):
	WcxBase(library_name),
	m_arc_info(){}
	virtual ~Wcx(){}

private:
	std::vector<fileinfo::FILEINFO> m_arc_info;

private:
	//ヘッダーから情報を取得
	fileinfo::FILEINFO data();
	//書庫内のすべてのファイルの情報を取得
	bool createFilesList(const TCHAR* arc_path);
	//エラーメッセージを取得
	bool getErrorMessage(tstring* msg,int code);

public:
	inline ARC_TYPE type(){return isWcx()?WCX:UNKNOWN;}
	//wcxである
	bool isWcx();
	//対応している拡張子であるか
	bool isSupportedExtension(const TCHAR* ext);
	//対応している書庫か
	bool isSupportedArchive(const TCHAR* arc_path, int mode=0);
	//設定ダイアログを表示
	bool configurationDialog(HWND wnd_handle=NULL);
	int getFileCount(const TCHAR* arc_path);
	ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir_orig,tstring* log_msg=NULL);
	ARC_RESULT list(const TCHAR* arc_path);
	ARC_RESULT test(const TCHAR* arc_path);

	//作成しようとするディレクトリは不要であるかどうか
	bool isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file);
};

#endif //_WCX_H_1948985C_769E_421b_957B_71F189EAA154
