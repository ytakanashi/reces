//ArcTar32.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r21 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCTAR32_H_F6AD9D40_6466_4578_93AA_C6FEBAFBFF26
#define _ARCTAR32_H_F6AD9D40_6466_4578_93AA_C6FEBAFBFF26

#include"ArcCfg.h"
#include"ArcDll.h"



class ArcTar32:public ArcDll{
public:
enum{
	ARCHIVETYPE_NORMAL=0,
	ARCHIVETYPE_TAR=1,
	ARCHIVETYPE_TARGZ=2,
	ARCHIVETYPE_TARZ=3,
	ARCHIVETYPE_GZ=4,
	ARCHIVETYPE_Z=5,
	ARCHIVETYPE_TARBZ2=6,
	ARCHIVETYPE_BZ2=7,
	ARCHIVETYPE_TARLZMA=8,
	ARCHIVETYPE_TARXZ=9,
	ARCHIVETYPE_LZMA=10,
	ARCHIVETYPE_XZ=11,

	ARCHIVETYPE_CPIO=32,
	ARCHIVETYPE_CPIOGZ=32+4,
	ARCHIVETYPE_CPIOZ=32+5,
	ARCHIVETYPE_CPIOBZ2=32+7,
	ARCHIVETYPE_CPIOLZMA=32+10,
	ARCHIVETYPE_CPIOXZ=32+11,

	ARCHIVETYPE_AR=48,
	ARCHIVETYPE_ARGZ=48+4,
	ARCHIVETYPE_ARZ=48+5,
	ARCHIVETYPE_ARBZ2=48+7,
	ARCHIVETYPE_ARLZMA=48+10,
	ARCHIVETYPE_ARXZ=48+11
};
public:
	ArcTar32(ArcCfg& arc_cfg);
	~ArcTar32(){}

private:
	ArcCfg& m_arc_cfg;
	long long m_file_size;
	DWORD m_write_size;
	DWORD m_last_write_size;
	typedef std::pair<tstring,long long> size_info;

protected:
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);

	//ファイル処理情報を格納
	void setExtractingInfo(UINT state,void* arc_info);

public:
	//圧縮形式を取得(その形式に対応している場合のみ)
	tstring getCompressionMethod(const TCHAR* arc_path_orig);

	ARCDLL_RESULT compress(const TCHAR* arc_path_orig,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARCDLL_RESULT extract(const TCHAR* arc_path_orig,const TCHAR* output_dir_orig,tstring* log_msg=NULL);
	void list(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
};
#endif //_ARCTAR32_H_F6AD9D40_6466_4578_93AA_C6FEBAFBFF26
