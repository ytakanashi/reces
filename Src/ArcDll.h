//ArcDll.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r27 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCDLL_H_31B31910_3688_4592_9BC6_FF763D854A5C
#define _ARCDLL_H_31B31910_3688_4592_9BC6_FF763D854A5C

#include"ArcDllBase.h"
#include"FileInfo.h"
#include"Msg.h"


namespace callback{
	inline BOOL __stdcall proc(HWND wnd_handle,UINT msg,UINT state,void* info);
}

class ArcDllUtil;

class ArcDll:public ArcDllBase{
friend class ArcFileSearch;
friend class ArcDllUtil;
public:
	ArcDll(const TCHAR* library_name,const TCHAR* library_prefix,const TCHAR* supported_ext,const TCHAR* delimiter);
	virtual ~ArcDll();

public:
	enum METHOD_OPTIONS{
		MHD_PASSWORD=1<<0,
		MHD_HEADERENCRYPTION=1<<1,
		MHD_SFX=1<<2,
	};

	//統合アーカイバライブラリである
	bool isCAL();

	//プラグインについての情報を取得
	tstring getInformation();
	//対応している圧縮形式であるか
	bool isSupportedMethod(const TCHAR* mhd);
	//対応している拡張子であるか
	bool isSupportedExtension(const TCHAR* ext);
	//対応している書庫であるか
	virtual bool isSupportedArchive(const TCHAR* arc_path,int mode=CHECKARCHIVE_BASIC);
	//圧縮形式を取得(その形式に対応している場合のみ)
	virtual tstring getCompressionMethod(const TCHAR* arc_path);


protected:
	unsigned int m_extracting_info_struct_size;
	ARC_PROCESSING_INFO m_processing_info;

	struct COMPRESSION_METHOD{
		const TCHAR* mhd;
		const TCHAR* ext;
		const TCHAR* cmd;
		const TCHAR* level;
		DWORD opt;
		int defualt_level;
		int minimum_level;
		int maximum_level;
	};
	std::vector<COMPRESSION_METHOD> m_compression_methods;
	size_t m_method_index;

	//対応拡張子リスト
	std::list<tstring> m_supported_ext_list;

	TCHAR m_delimiter[4];

	const int log_buffer_size;

	//書庫情報
	struct ARC_INFO{
		tstring arc_path;
		std::vector<fileinfo::FILEINFO> file_list;
		long long original_size;
		struct RD{
			bool double_dir;
			bool only_file;
			bool checked;
			RD():double_dir(false),only_file(false),checked(false){}
			void clear(){double_dir=only_file=checked=false;}
		}rd;

		ARC_INFO():
			arc_path(),file_list(),original_size(0),rd(){}
		bool empty()const{return arc_path.empty()&&file_list.empty();}
		bool isCreated(const TCHAR* name)const{return arc_path==name&&!file_list.empty();}
		void doubleDir(bool f){rd.checked=true;rd.double_dir=f;}
		void onlyFile(bool f){rd.checked=true;rd.only_file=f;}
		bool rdChecked()const{return rd.checked;}
		bool isDoubleDir()const{return rd.double_dir;}
		bool isOnlyFile()const{return rd.only_file;}
		void clear(){
			arc_path.clear();
			file_list.clear();
			original_size=0;
			rd.clear();
		}
	};
	ARC_INFO m_arc_info;

	//パスの区切り文字をそれぞれのライブラリにあったものに置換
	void replaceDelimiter(TCHAR* str);
	void replaceDelimiter(tstring& str);
	void replaceDelimiter(std::list<tstring>* list);

	//二重引用符でパスを囲む
	tstring quotePath(const tstring& path);

	//ファイル処理情報を格納
	virtual void setExtractingInfo(/*UINT state,*/void* arc_info);
	//コールバック通知を無効にする
	bool m_disable_callback;

	//ArcDll用便利関数たち
	ArcDllUtil* m_util;

	//書庫内のすべてのファイルの情報を取得
	bool createFilesList(const TCHAR* arc_path);

	//リストにフィルタを適用
	virtual void applyFilters(std::vector<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse=false);

	enum OUTPUTFILELIST_OPT{
		REVERSE_FILTER=1<<0
	};

	//リストファイルにファイルリストを出力
	virtual void outputFileListToFile(const fileinfo::FILEINFO& fileinfo,int opt=0,sslib::File* list_file=NULL);
	//コンソールにファイルリストを出力
	virtual bool outputFileListToConsole(const fileinfo::FILEINFO& fileinfo,int opt=0);

	//ファイルリストを出力
	virtual bool outputFileList(const std::vector<fileinfo::FILEINFO>& fileinfo_list,int opt=0,sslib::File* list_file=NULL);

	//実装してねゾーン
	//--[ここから]--
	//書庫内ファイルのパスを整形してファイルに書き出す
	virtual DWORD writeFormatedPath(const sslib::File& list_file,const TCHAR* base_dir,const TCHAR* file_path)=0;
	//書庫内ファイルリストを整形してファイルに書き出す
	virtual bool writeFormatedList(const sslib::File& list_file,const tstring& full_path)=0;
	//--[ここまで]--

public:
	inline ARC_TYPE type(){return isCAL()?CAL:UNKNOWN;}

	//設定ダイアログを表示
	virtual bool configurationDialog(HWND wnd_handle=NULL);

	virtual ARC_RESULT del(const TCHAR* arc_path_orig,tstring* log_msg=NULL);
	virtual ARC_RESULT test(const TCHAR* arc_path);
	virtual int sendCommands(const TCHAR* commands,tstring* log_msg=NULL);

protected:
	bool callbackProcV(HWND wnd_handle,UINT msg,/*UINT state,*/void* info);
private:
	friend BOOL __stdcall callback::proc(HWND wnd_handle,UINT msg,UINT state,void* info);

	//ライブラリから情報を受け取るコールバック関数を設定
	bool setArchiveProc();

	//書庫を開く
	bool open(const TCHAR* file_name,const DWORD mode=0);
	//書庫を閉じる
	bool close();

public:
	//指定された書庫ファイルに格納されているファイル数を得ます
	int getFileCount(const TCHAR* arc_path);

	//書庫に含まれるファイルのサイズの合計を取得
	long long getTotalOriginalSize(const TCHAR* arc_path);

	//Dllのバージョンを表示
	tstring getVersionStr();

	//ライブラリから情報を受け取るスレッドを設定
	bool setCallback(unsigned int progress_thread_id);
	//コールバックの設定を解除
	void clearCallback();

	//書庫に含まれるファイルの一覧を出力
	bool outputFileList(const TCHAR* arc_path,int opt=0,sslib::File* list_file=NULL);

	//書庫に含まれるファイルの一覧を出力(フィルタ指定可能)
	bool outputFileListEx(const TCHAR* arc_path,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,int opt=0,sslib::File* list_file=NULL);

	//作成しようとするディレクトリは不要であるかどうか
	bool isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file);

	//圧縮形式その他情報を取得
	inline const COMPRESSION_METHOD& getMethod()const{return m_compression_methods[m_method_index];}
	inline const COMPRESSION_METHOD& getMethod(size_t index)const{return m_compression_methods[index];}
};

class ArcFileSearch{
public:
	ArcFileSearch(ArcDll* arcdll_ptr):m_arcdll_ptr(arcdll_ptr),m_individual_info(),m_fileinfo(){
	}
	virtual ~ArcFileSearch(){}
private:
	ArcDll* m_arcdll_ptr;
	INDIVIDUALINFO m_individual_info;
	fileinfo::FILEINFO m_fileinfo;
private:
	//各種構造体のデータより属性を推測
	//(ライブラリや書庫によってszAttributeが異なるため保証なし...)
	DWORD guessAttribute(const char* attr_str_orig,const char* file_name_orig){
		DWORD result=0;
		tstring file_name((m_arcdll_ptr->isUnicodeMode())?sslib::str::utf82utf16(file_name_orig):sslib::str::sjis2utf16(file_name_orig));
		tstring attr_str(sslib::str::toLower((m_arcdll_ptr->isUnicodeMode())?sslib::str::utf82utf16(attr_str_orig):sslib::str::sjis2utf16(attr_str_orig)));

		if(attr_str.find(_T("r"))!=tstring::npos){
			result|=FILE_ATTRIBUTE_READONLY;
		}

		if(attr_str.find(_T("h"))!=tstring::npos){
			result|=FILE_ATTRIBUTE_HIDDEN;
		}

		if(attr_str.find(_T("s"))!=tstring::npos){
			result|=FILE_ATTRIBUTE_SYSTEM;
		}

		if(attr_str.find(_T("d"))!=tstring::npos||
		   //ライブラリによっては主に使用するデリミタとリスト表示のデリミタが異なる場合有り
		   file_name.find_last_of(_T("\\/"))==file_name.length()-1){
			result|=FILE_ATTRIBUTE_DIRECTORY;
		}
		return result;//(result==0)?FILE_ATTRIBUTE_NORMAL:result;
	}
public:
	inline bool first(const TCHAR* wild_name=_T("*")){
		return (m_arcdll_ptr&&m_arcdll_ptr->findFirst(wild_name,&m_individual_info))?false:true;
	}
	inline bool next(){
		return (m_arcdll_ptr&&m_arcdll_ptr->findNext(&m_individual_info)==0)?true:false;
	}
	//書庫内ファイル検索(オープン/クローズは各自で)
	fileinfo::FILEINFO* getFileInfo(){
		//ファイルサイズ
		if(!m_arcdll_ptr->getOriginalSizeEx(&m_fileinfo.size)){
			m_fileinfo.size=m_individual_info.dwOriginalSize;
		}

		//日付/時刻
		FILETIME ft1={},ft2={};
		SYSTEMTIME st={};

		::DosDateTimeToFileTime(m_individual_info.wDate,m_individual_info.wTime,&ft1);
		::LocalFileTimeToFileTime(&ft1,&ft2);
		::FileTimeToSystemTime(&ft2,&st);
		m_fileinfo.date_time=sslib::strex::SYSTEMTIME2longlong(st);

		//ファイル名
		tstring buffer(1024,'\0');

		if(m_arcdll_ptr->queryFunctionList(ISARC_GET_FILE_NAME)){
			m_arcdll_ptr->getFileName(&buffer,1024);
			m_fileinfo.name.assign(buffer);
		}else{
			//GetFileName()が実装されていない場合、INDIVIDUALINFO構造体より取得
			if(m_arcdll_ptr->isUnicodeMode()){
				sslib::str::utf82utf16(&m_fileinfo.name,m_individual_info.szFileName);
			}else{
				sslib::str::sjis2utf16(&m_fileinfo.name,m_individual_info.szFileName);
			}
		}

		//属性
		DWORD attr=0;

		if(!m_arcdll_ptr->getAttribute(&attr)){
			//GetAttribute()が実装されていない場合、INDIVIDUALINFO構造体より取得
			attr=guessAttribute(m_individual_info.szAttribute,m_individual_info.szFileName);
		}
		m_fileinfo.attr=attr;

		return &m_fileinfo;
	}
};

class ArcDllUtil{
public:
	ArcDllUtil(ArcDll* arcdll_ptr):m_arcdll_ptr(arcdll_ptr){
	}
	virtual ~ArcDllUtil(){}
private:
	ArcDll* m_arcdll_ptr;
public:
	//ディレクトリのタイムスタンプを復元
	bool recoverDirectoryTimestamp(const TCHAR* arc_path,const TCHAR* output_dir_orig,bool no_arc_list=false);
	//共通パスを取り除く
	//解凍レンジ「パス情報を最適化して展開する」相当
	//level=-1で共通パスをすべて取り除く
	bool excludeCommonPath(const TCHAR* output_dir,const TCHAR* target_dir,int level=-1);
};

#endif //_ARCDLL_H_31B31910_3688_4592_9BC6_FF763D854A5C
