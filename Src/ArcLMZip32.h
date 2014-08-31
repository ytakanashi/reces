//ArcLMZip32.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r22 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCLMZIP32_H_F6AD9D40_6466_4578_93AA_C6FEBAFBFF26
#define _ARCLMZIP32_H_F6AD9D40_6466_4578_93AA_C6FEBAFBFF26

#include"ArcCfg.h"
#include"ArcDll.h"



class ArcLMZip32:public ArcDll{
public:
	ArcLMZip32(ArcCfg& arc_cfg);
	~ArcLMZip32(){}

private:
	ArcCfg& m_arc_cfg;
	long long m_file_size;
	typedef std::pair<tstring,long long> size_info;

protected:
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);

	//ファイル処理情報を格納
	void setExtractingInfo(UINT state,void* arc_info);

public:
	ARCDLL_RESULT compress(const TCHAR* arc_path_orig,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARCDLL_RESULT extract(const TCHAR* arc_path_orig,const TCHAR* output_dir_orig,tstring* log_msg=NULL);
	void list(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
	bool test(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
	int sendCommands(const TCHAR* commands,tstring* log_msg=NULL);

	//処理を中止する
	void abort();

	//ArcDll::callbackProcV()に投げるための準備
	bool preCallback(const TCHAR* file_name,const TCHAR* progress);
};
#endif //_ARCLMZIP32_H_F6AD9D40_6466_4578_93AA_C6FEBAFBFF26
