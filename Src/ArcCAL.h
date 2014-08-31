//ArcCAL.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r22 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCCAL_H_7FC5D2FE_6566_43f5_99ED_0BFB53F82DE9
#define _ARCCAL_H_7FC5D2FE_6566_43f5_99ED_0BFB53F82DE9

#include"ArcCfg.h"
#include"ArcDll.h"



class ArcCAL:public ArcDll{
public:
	ArcCAL(ArcCfg& arc_cfg,const TCHAR* library_name,const TCHAR* library_prefix);
	~ArcCAL(){}

private:
	ArcCfg& m_arc_cfg;

protected:
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);
public:
	ARCDLL_RESULT compress(const TCHAR* arc_path_orig,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARCDLL_RESULT extract(const TCHAR* arc_path_orig,const TCHAR* output_dir_orig,tstring* log_msg=NULL);
	void list(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
};
#endif //_ARCCAL_H_7FC5D2FE_6566_43f5_99ED_0BFB53F82DE9
