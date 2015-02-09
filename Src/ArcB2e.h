//ArcB2e.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCB2E_H_3F99C027_3723_46ab_8320_2B24426F74F6
#define _ARCB2E_H_3F99C027_3723_46ab_8320_2B24426F74F6

#include"ArcDll.h"



class ArcB2e:public ArcDll{
public:
	enum B2E_ABILITY{
		B2E_ABILITY_NONE=0,
		B2E_ABILITY_CHECK=1<<0,//CheckArchive()相当の処理が可能
		B2E_ABILITY_MELT=1<<1,//解凍処理が可能
		B2E_ABILITY_LIST=1<<2,//書庫内ファイルの列挙が可能
		B2E_ABILITY_MELT_EACH=1<<3,//指定したファイルのみの解凍が可能
		B2E_ABILITY_COMPRESS=1<<4,//圧縮が可能
		B2E_ABILITY_ARCHIVE=1<<5,//複数ファイルをまとめることが可能(ex.GZip)
		B2E_ABILITY_SFX=1<<6,//自己解凍ファイルを作成可能
		B2E_ABILITY_ADD=1<<7,//ファイルの追加が可能
		B2E_ABILITY_DELETE=1<<8,//ファイルの削除が可能
	};

public:
	ArcB2e();
	~ArcB2e(){}

protected:
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);
public:
	ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg=NULL);
	ARC_RESULT list(const TCHAR* arc_path);

	//b2eスクリプトのあるディレクトリを指定
	bool setScriptDirectory(const TCHAR* dir_path);
	//B2Eスクリプトの数を取得
	int getScriptCount();
	//b2eスクリプト名を取得
	bool getScriptName(const UINT index,tstring* result_buffer,DWORD buffer_size);
	//b2eスクリプトの能力を列挙
	B2E_ABILITY getScriptAbility(const UINT index);
	//解凍できる拡張子('.'付き)の_nExtIndex番目のものを取得
	bool getExtractExtensions(const UINT index,const UINT ext_index,tstring* result_buffer,DWORD buffer_size);
	//b2eの圧縮対応形式名を取得(拡張子名でないことに注意)
	bool getCompressType(const UINT index,tstring* result_buffer,DWORD buffer_size);
	//圧縮用b2eの対応メソッドを取得
	bool getCompressMethod(const UINT index,const UINT mhd_index,tstring* result_buffer,DWORD buffer_size);
	//圧縮用B2Eのデフォルトメソッド番号を取得
	int getDefaultCompressMethod(const UINT index);
	//指定したアーカイブファイルの解凍に使うB2Eのインデックスを取得
	UINT getExtractorIndex(const TCHAR* file_path);
};
#endif //_ARCB2E_H_3F99C027_3723_46ab_8320_2B24426F74F6
