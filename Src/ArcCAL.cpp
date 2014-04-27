//ArcCAL.cpp
//対応外の統合アーカイバライブラリ操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r20 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcCAL.h"

using namespace sslib;


ArcCAL::ArcCAL(ArcCfg& arc_cfg,const TCHAR* library_name,const TCHAR* library_prefix):
	ArcDll(library_name,
			library_prefix,
			_T("*"),
			_T("\\")),
	m_arc_cfg(arc_cfg){
		COMPRESSION_METHOD method[]={
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_methods.assign(method,method+ARRAY_SIZEOF(method));
}

ArcDll::ARCDLL_RESULT ArcCAL::compress(const TCHAR* arc_path_orig,std::list<tstring>* file_list,tstring* log_msg){
	return ARCDLL_FAILURE;
}

ArcDll::ARCDLL_RESULT ArcCAL::extract(const TCHAR* arc_path_orig,const TCHAR* output_dir_orig,tstring* log_msg){
	return ARCDLL_FAILURE;
}

void ArcCAL::list(const TCHAR* arc_path_orig,tstring* log_msg){
	return;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcCAL::writeFormatedPath(const File& list_file,const TCHAR* base_dir,const TCHAR* file_path){
	return -1;
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcCAL::writeFormatedList(const File& list_file,const tstring& full_path){
	return false;
}
