//ArcUnrar32.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r34 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCUNRAR32_H_A541EA38_5734_4b18_88BC_F731657CCEBC
#define _ARCUNRAR32_H_A541EA38_5734_4b18_88BC_F731657CCEBC

#include"ArcDll.h"


class ArcUnrar32:public ArcDll{
public:
	ArcUnrar32();
	~ArcUnrar32(){}

protected:
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);

	//ファイル処理情報を格納
	void setExtractingInfo(/*UINT state,*/void* arc_info);

	//関数対応を操作
	bool querySupport(int func);

public:
	//対応している書庫であるか
	bool isSupportedArchive(const TCHAR* arc_path_orig,int mode=CHECKARCHIVE_BASIC);
	ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg=NULL);
	ARC_RESULT list(const TCHAR* arc_path);
	ARC_RESULT test(const TCHAR* arc_path);

	//処理を中止する
	void abort();

	//ArcDll::callbackProcV()に投げるための準備
	bool preCallback(const TCHAR* file_name,const TCHAR* progress);
};
#endif //_ARCUNRAR32_H_A541EA38_5734_4b18_88BC_F731657CCEBC
