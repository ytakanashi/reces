//Arc7zip32.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARC7ZIP32_H_522A5F4B_4030_4152_9D18_041AC82BE8B0
#define _ARC7ZIP32_H_522A5F4B_4030_4152_9D18_041AC82BE8B0

#include"ArcDll.h"
#include"FileInfo.h"
#include<utility>


class Arc7zip32:public ArcDll{
public:
enum{
	ARCHIVETYPE_ZIP=1,
	ARCHIVETYPE_7Z=2
};
public:
	Arc7zip32();
	~Arc7zip32(){};

private:
	long long m_file_size;
	long long m_write_size;
	typedef std::pair<tstring,long long> size_info;

protected:
	//リストにフィルタを適用
	void applyFilters(std::vector<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse=false);

	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path);
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* file_path);

	//圧縮対象ファイルリストを整形してファイルに書き出す(ダミー)
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);
	//圧縮対象ファイルリストを整形してファイルに書き出す(処理対象外リストも書き出す)
	bool writeFormatedList(const sslib::File& list_file,sslib::File& exclude_list_file,const tstring& full_path);

public:
	//対応している書庫であるか
	bool isSupportedArchive(const TCHAR* arc_path_orig,const DWORD mode=CHECKARCHIVE_BASIC);
	//圧縮形式を取得(その形式に対応している場合のみ)
	tstring getCompressionMethod(const TCHAR* arc_path_orig);

	ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg=NULL);
	ARC_RESULT del(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
	ARC_RESULT list(const TCHAR* arc_path);
	ARC_RESULT test(const TCHAR* arc_path);
};
#endif //_ARC7ZIP32_H_522A5F4B_4030_4152_9D18_041AC82BE8B0
