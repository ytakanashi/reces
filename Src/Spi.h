//Spi.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r32 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _SPI_H_4F4E72AA_0C47_4f12_8683_6D9F77D3AE58
#define _SPI_H_4F4E72AA_0C47_4f12_8683_6D9F77D3AE58


#include"SpiBase.h"
#include"PrivateProfile.h"
#include"FileInfo.h"


class Spi:public SpiBase{
public:
	Spi(const TCHAR* library_name):
	SpiBase(library_name),
	m_arc_info(){}
	virtual ~Spi(){}

public:
	//ライブラリを読み込みます
	virtual bool load();
	virtual bool load(const TCHAR* library_name,const TCHAR* library_prefix);

private:
	std::vector<fileinfo::FILEINFO> m_arc_info;
	//対応拡張子リスト
	std::list<tstring> m_supported_ext_list;

private:
	//書庫内のすべてのファイルの情報を取得
	bool createFilesList(const TCHAR* arc_path);
	//エラーメッセージを取得
	bool getErrorMessage(tstring* msg,int code);

public:
	inline ARC_TYPE type(){return static_cast<ARC_TYPE>(isSusiePlugin());}
	//spiである[戻り値はプラグインのタイプ]
	int isSusiePlugin();
	//対応している拡張子であるか
	bool isSupportedExtension(const TCHAR* ext);
	//対応している書庫か
	bool isSupportedArchive(const TCHAR* arc_path,int mode=0);
	//プラグインに就いての情報を取得
	tstring getInformation();
	//設定ダイアログを表示
	bool configurationDialog(HWND wnd_handle=NULL);
	int getFileCount(const TCHAR* arc_path);
	ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir_orig,tstring* log_msg=NULL);
	ARC_RESULT list(const TCHAR* arc_path);
	ARC_RESULT test(const TCHAR* arc_path);

	//作成しようとするディレクトリは不要であるかどうか
	bool isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file,tstring* root_dir=NULL);
};

#endif //_SPI_H_4F4E72AA_0C47_4f12_8683_6D9F77D3AE58
