//Archiver.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r34 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCHIVER_H_B97F1BB8_B019_40f2_9646_8B52530658FF
#define _ARCHIVER_H_B97F1BB8_B019_40f2_9646_8B52530658FF


#include"FileInfo.h"

class Archiver:public sslib::Library{
public:
	Archiver():
		m_progress_thread_id(0),
		m_background_mode(false){}
	Archiver(const TCHAR* library_name);
	Archiver(const TCHAR* library_name,const TCHAR* library_prefix);
	virtual ~Archiver(){}

public:
	enum ARC_RESULT{
		//処理成功
		ARC_SUCCESS,
		//リストファイルを開くことが出来ない
		ARC_CANNOT_OPEN_LISTFILE,
		//マッチするファイルがない
		ARC_NO_MATCHES_FOUND,
		//機能が実装されていない
		ARC_NOT_IMPLEMENTED,
		//フィルタが指定されていない
		ARC_NO_FILTER,
		//処理失敗
		ARC_FAILURE
	};

	struct ARC_PROCESSING_INFO{
		long long done;
		long long total;
		tstring file_name;
		ARC_PROCESSING_INFO(long long done_,long long total_,const tstring& file_name_):
			done(done_),total(total_),file_name(file_name_){}
		ARC_PROCESSING_INFO():
			done(0),total(0),file_name(){}
		void clear(){done=total=0;file_name.clear();}
	};

	enum ARC_TYPE{
		//統合アーカイバ
		CAL=1,
		//B2e32.dll
		B2E,
		//書庫用Susie Plug-in
		SPI_AM,
		//画像用Susie Plug-in
		SPI_IN,
		//書庫用Total Commander Plugin
		WCX,
		//よくわからないもの
		UNKNOWN=-1,
	};

	enum{
		default_compressionlevel=-1,
		minimum_compressionlevel=-2,
		maximum_compressionlevel=-3
	};

protected:
	//プログレスバー更新の通知先
	unsigned int m_progress_thread_id;
	//バックグラウンドモードか否か
	bool m_background_mode;

public:
	//プログレスバー更新通知
	static const UINT WM_UPDATE_PROGRESSBAR;

protected:
	//書庫内の最小共通区切り文字数を取得
	int countDelimiter(const TCHAR* arc_path,const std::vector<fileinfo::FILEINFO>& arc_info);

public:
	//実装してねゾーン
	//--[ここから]--
	virtual ARC_RESULT compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg=NULL)=0;
	virtual ARC_RESULT extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg=NULL)=0;
	virtual ARC_RESULT list(const TCHAR* arc_path)=0;
	virtual ARC_RESULT test(const TCHAR* arc_path)=0;
	virtual ARC_TYPE type()=0;
	//対応している拡張子であるか
	virtual bool isSupportedExtension(const TCHAR* ext)=0;
	//対応している書庫か
	virtual bool isSupportedArchive(const TCHAR* arc_path,int mode=0)=0;
	//設定ダイアログを表示
	virtual bool configurationDialog(HWND wnd_handle=NULL)=0;
	virtual int getFileCount(const TCHAR* arc_path)=0;
	//書庫内のすべてのファイルの情報を取得
	virtual bool createFilesList(const TCHAR* arc_path)=0;
	//作成しようとするディレクトリは不要であるかどうか
	virtual bool isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file,tstring* root_dir=NULL)=0;
	//--[ここまで]--
	//プラグインについての情報を取得
	virtual tstring getInformation();
	//ライブラリから情報を受け取るスレッドを設定
	virtual bool setCallback(unsigned int progress_thread_id);
	//コールバックの設定を解除
	virtual void clearCallback();
#if 0
	//バックグラウンドモードか否か
	virtual bool getBackgroundMode();
#endif
	//バックグラウンドモードを設定
	virtual bool setBackgroundMode(bool mode=false);
	//処理を中止する
	virtual void abort();

};

class RedundantDir{
public:
	RedundantDir():
		m_is_double_dir(true),
		m_has_dir(false),
		m_file_in_root_dir(false),
		m_root_dir(){}
	virtual ~RedundantDir(){}
private:
	bool m_is_double_dir;
	bool m_has_dir;
	bool m_file_in_root_dir;
	tstring m_root_dir;
public:
	inline bool isDoubleDir()const{return m_is_double_dir;}
	inline bool hasDir()const{return m_has_dir;}
	inline const tstring root_dir()const{return m_root_dir;}
	//true=break/false=continue
	bool operator()(const fileinfo::FILEINFO& fileinfo);
};

//配下のファイルを利用してディレクトリのタイムスタンプを復元
namespace dirtimestamp{
	bool recover(const TCHAR* dir,const FILETIME& arc_ft,const std::vector<tstring>& recovered_dirs);
	bool addIncompleteDir(const tstring& output_dir,const tstring& name,std::vector<tstring>* incomplete_dirs);
}

namespace filter{
	//属性フィルタにマッチするか
	bool matchAttributes(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool delimiter_check=true);
	//フィルタにマッチするか
	bool match(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter);
}

#endif //_ARCHIVER_H_B97F1BB8_B019_40f2_9646_8B52530658FF
