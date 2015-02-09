//ArcXacrett.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCXACRETT_H_A614B220_8D60_42a3_A755_319198605FF7
#define _ARCXACRETT_H_A614B220_8D60_42a3_A755_319198605FF7

#include"ArcDll.h"




class ArcXacrett:public ArcDll{
public:
enum{
	ARCHIVETYPE_UNKNOWN=0,
	ARCHIVETYPE_ACE=1,
	ARCHIVETYPE_ARC=2,
	ARCHIVETYPE_ARG=3,
	ARCHIVETYPE_ARJ=4,
	ARCHIVETYPE_AR=5,
	ARCHIVETYPE_ASD=6,
	ARCHIVETYPE_ATOB=7,
	ARCHIVETYPE_BEL=8,
	ARCHIVETYPE_BGA=9,
	ARCHIVETYPE_BOO=10,
	ARCHIVETYPE_BZ2=11,
	ARCHIVETYPE_CAB=12,
	ARCHIVETYPE_COMPACT=13,
	ARCHIVETYPE_COMPRESSZ=14,
	ARCHIVETYPE_CPIO=15,
	ARCHIVETYPE_CPT=16,
	ARCHIVETYPE_DZ=17,
	ARCHIVETYPE_FISH=18,
	ARCHIVETYPE_FRZ=19,
	ARCHIVETYPE_GCA=20,
	ARCHIVETYPE_GZ=21,
	ARCHIVETYPE_HQX=22,
	ARCHIVETYPE_IMP=23,
	ARCHIVETYPE_ISH=24,
	ARCHIVETYPE_IS=25,
	ARCHIVETYPE_JAK=26,
	ARCHIVETYPE_JAM=27,
	ARCHIVETYPE_LZH=28,
	ARCHIVETYPE_MACBIN=29,
	ARCHIVETYPE_MIME=30,
	ARCHIVETYPE_MSCOMPRESS=31,
	ARCHIVETYPE_PACKZ=32,
	ARCHIVETYPE_PIT=33,
	ARCHIVETYPE_QPAC=34,
	ARCHIVETYPE_RAR=35,
	ARCHIVETYPE_REZ=36,
	ARCHIVETYPE_SHAR=37,
	ARCHIVETYPE_SPL=38,
	ARCHIVETYPE_TAR=39,
	ARCHIVETYPE_UUDECODE=40,
	ARCHIVETYPE_YNC=41,
	ARCHIVETYPE_ZAC=42,
	ARCHIVETYPE_ZIP=43,
	ARCHIVETYPE_ZOO=44,
	ARCHIVETYPE_YZ1=45,
	ARCHIVETYPE_TBZ=46,
	ARCHIVETYPE_TGZ=47,
	ARCHIVETYPE_TAZ=48,
	ARCHIVETYPE_TAz=49,
	ARCHIVETYPE_CPIOGZRPM=50,
	ARCHIVETYPE_CPIOZ=51,
	ARCHIVETYPE_CPIOBZ2=52,
	ARCHIVETYPE_XXDECODE=53,
	ARCHIVETYPE_BLZ=54,
	ARCHIVETYPE_ALZ=55,
	ARCHIVETYPE_DMG=56,
	ARCHIVETYPE_ISO=57,
	ARCHIVETYPE_LZX=58,
	ARCHIVETYPE_JC1=59,
	ARCHIVETYPE_APK=60,
	ARCHIVETYPE_SQX=61,
	ARCHIVETYPE_ASK=62
};
public:
	ArcXacrett();
	~ArcXacrett(){}

private:
	tstring* m_log_msg;
	long long m_file_size;
	long long m_write_size;

protected:
	//圧縮対象ファイルのパスを整形してファイルに書き出す
	DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path);
	//圧縮対象ファイルリストを整形してファイルに書き出す
	bool writeFormatedList(const sslib::File& list_file,const tstring& full_path);

	//ファイル処理情報を格納
	void setExtractingInfo(UINT state,void* arc_info);
public:
	//書庫形式を取得
//	bool getCompressionMethod(TCHAR* type,const TCHAR* arc_path);

	ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL);
	ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg=NULL);
	ARC_RESULT list(const TCHAR* arc_path);

	//処理を中止する
	void abort();

	//ArcDll::callbackProcV()に投げるための準備
	bool preCallback(const char* file_name);
};
#endif //_ARCXACRETT_H_A614B220_8D60_42a3_A755_319198605FF7
