//ArcCAL.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r33 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCCAL_H_7FC5D2FE_6566_43f5_99ED_0BFB53F82DE9
#define _ARCCAL_H_7FC5D2FE_6566_43f5_99ED_0BFB53F82DE9

#include"ArcDll.h"



class ArcCAL:public ArcDll{
public:
	ArcCAL(const TCHAR* library_name,const TCHAR* library_prefix);
	~ArcCAL(){}

protected:
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);
public:
	ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg=NULL);
	ARC_RESULT list(const TCHAR* arc_path);
};
#endif //_ARCCAL_H_7FC5D2FE_6566_43f5_99ED_0BFB53F82DE9
