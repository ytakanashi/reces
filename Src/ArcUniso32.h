//ArcUniso32.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r20 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCUniso32_H_522A5F4B_4030_4152_9D18_041AC82BE8B0
#define _ARCUniso32_H_522A5F4B_4030_4152_9D18_041AC82BE8B0

#include"ArcCfg.h"
#include"ArcDll.h"
#include"PrivateProfile.h"
#include<utility>

class ArcUniso32:public ArcDll{
public:
	ArcUniso32(ArcCfg& arc_cfg);
	~ArcUniso32(){}

private:
	ArcCfg& m_arc_cfg;

protected:
	//リストにフィルタを適用
	void applyFilters(std::list<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse=false);

	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path);
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* file_path);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);

public:
	ARCDLL_RESULT compress(const TCHAR* arc_path_orig,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARCDLL_RESULT extract(const TCHAR* arc_path_orig,const TCHAR* output_dir_orig,tstring* log_msg=NULL);
	void list(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
	bool test(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
};
#endif //_ARCUniso32_H_522A5F4B_4030_4152_9D18_041AC82BE8B0
