//Spi.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r20 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#ifndef _SPI_H_4F4E72AA_0C47_4f12_8683_6D9F77D3AE58
#define _SPI_H_4F4E72AA_0C47_4f12_8683_6D9F77D3AE58


#include"SpiBase.h"
#include"ArcCfg.h"
#include"ArcCommon.h"
#include"PrivateProfile.h"
#include"FileInfo.h"


class Spi:public SpiBase,public ArcCommon{
public:
	Spi(const TCHAR* library_name,ArcCfg& arc_cfg):
	SpiBase(library_name),
	ArcCommon(),
	m_arc_cfg(arc_cfg),
	m_arc_info(){}
	virtual ~Spi(){}

private:
	ArcCfg& m_arc_cfg;
	std::list<fileinfo::FILEINFO> m_arc_info;

private:
	//書庫内のすべてのファイルの情報を取得
	bool createFilesList(const TCHAR* arc_path);

public:
	//spiである
	bool isSusiePlugin();
	//対応している書庫か
	bool isSupportedArchive(const TCHAR* arc_path);
	//プラグインに就いての情報を取得
	tstring getAboutStr();
	int getFileCount(const TCHAR* arc_path);
	bool list(const TCHAR* arc_path);
	bool extract(const TCHAR* arc_path,const TCHAR* output_dir_orig,unsigned int progress_thread_id=0);

	//作成しようとするディレクトリは不要であるかどうか
	bool isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file);
};

#endif //_SPI_H_4F4E72AA_0C47_4f12_8683_6D9F77D3AE58
