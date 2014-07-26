//ArcUnlha32.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r21 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCUNLHA32_H_78D1B6ED_452F_425d_B59E_41ABDDD903D7
#define _ARCUNLHA32_H_78D1B6ED_452F_425d_B59E_41ABDDD903D7

#include"ArcCfg.h"
#include"ArcDll.h"


class ArcUnlha32:public ArcDll{
public:
	ArcUnlha32(ArcCfg& arc_cfg);
	~ArcUnlha32(){}

private:
	ArcCfg& m_arc_cfg;
	long long m_file_size;
	long long m_write_size;
	long long m_last_write_size;
	DWORD m_last_hash;
	typedef std::pair<tstring,long long> size_info;

protected:
	//リストにフィルタを適用
	void applyFilters(std::list<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse=false);
	//リストファイルにファイルリストを出力
	void outputFileListToFile(const fileinfo::FILEINFO& fileinfo,const sslib::File& list_file,int opt=0);

	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig);
	//圧縮対象外ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedExcludePath(const sslib::File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);

	//ファイル処理情報を格納
	void setExtractingInfo(UINT state,void* arc_info);
public:
	ARCDLL_RESULT compress(const TCHAR* arc_path_orig,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARCDLL_RESULT extract(const TCHAR* arc_path_orig,const TCHAR* output_dir_orig,tstring* log_msg=NULL);
	ARCDLL_RESULT del(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
	void list(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
	bool test(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
};
#endif //_ARCUNLHA32_H_78D1B6ED_452F_425d_B59E_41ABDDD903D7
