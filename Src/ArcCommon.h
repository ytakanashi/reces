//ArcCommon.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r22 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCCOMMON_H_B97F1BB8_B019_40f2_9646_8B52530658FF
#define _ARCCOMMON_H_B97F1BB8_B019_40f2_9646_8B52530658FF


#include"FileInfo.h"

class ArcCommon{
public:
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

	ArcCommon():
	m_progress_thread_id(0){};
	~ArcCommon(){}

protected:
	//プログレスバー更新の通知先
	unsigned int m_progress_thread_id;

protected:
	//属性フィルタを適用(DIR属性が格納されていない書庫の為に...)
	bool applyAttributeFilters(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter);
	//フィルタにマッチするか
	bool matchFilters(const fileinfo::FILEINFO& fileinfo,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter);

	class RedundantDir{
	public:
		RedundantDir():
		m_is_double_dir(true),
		m_has_dir(false),
		m_checked_first_file(false),
		m_root_dir(){}
		~RedundantDir(){}
	private:
		bool m_is_double_dir;
		bool m_has_dir;
		bool m_checked_first_file;
		tstring m_root_dir;
	public:
		inline bool isDoubleDir()const{return m_is_double_dir;}
		inline bool hasDir()const{return m_has_dir;}
		//true=break/false=continue
		bool operator()(const fileinfo::FILEINFO& fileinfo);
	};

	//配下のファイルを利用してディレクトリのタイムスタンプを復元
	struct DirTimeStamp{
		static bool recover(const TCHAR* dir,const FILETIME& arc_ft,const std::vector<tstring>& recovered_dirs);
		static bool addIncompleteDir(const tstring& output_dir,const tstring& name,std::vector<tstring>* incomplete_dirs);
	};

	//共通パスを取り除く
	//解凍レンジ「パス情報を最適化して展開する」相当
	//level=-1で共通パスをすべて取り除く
	bool excludeCommonPath(const TCHAR* output_dir,const TCHAR* target_dir,int level=-1);

public:
	//プログレスバー更新通知
	static const UINT WM_UPDATE_PROGRESSBAR;

public:
	//処理を中止する
	virtual void abort();

};
#endif //_ARCCOMMON_H_B97F1BB8_B019_40f2_9646_8B52530658FF
