//reces.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r24 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681
#define _RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681

#include"ArcDll.h"
#include"ArcB2e.h"
#include"Spi.h"
#include"Wcx.h"
#include"PrivateProfile.h"



class Reces:public sslib::ConsoleApp{
public:
	Reces():
		m_arcdll_list(),
		m_b2e_dll(NULL),
		m_cal_dll(NULL),
		m_spi_list(),
		m_wcx_list(),
		m_split_temp_dir(),
		m_original_cur_dir(),
		m_hook_dll_module(NULL),
		m_hook_dll_path(),
		m_pInstallHook(NULL),
		m_pUninstallHook(NULL),
		m_progressbar(NULL),
		m_cur_file(),
		m_arc_thread(NULL),
		m_arc_dll(NULL),
		m_progressbar_thread(),
		m_dialog_hook_thread(){}
	~Reces(){}

private:
	enum ARC_RESULT{
		//処理成功
		ARC_SUCCESS,

		//対応していない書庫形式
		ARC_NOT_SUPPORTED_METHOD,
		//SFX+Sは選択できない
		ARC_INCORRECT_OPTION_SFX_S,
		//ライブラリの読み込みに失敗
		ARC_CANNOT_LOAD_LIBRARY,
		//他のプロセスで使用中
		ARC_LIBRARY_IS_BUSY,
		//ディレクトリの作成に失敗
		ARC_CANNOT_CREATE_DIRECTORY,
		//作成する書庫と同名のディレクトリが存在する
		ARC_DUPLICATE_NAME,

		//パスワードの入力がキャンセルされた
		ARC_INPUT_PASSWORD_CANCEL,

		//再圧縮ではなく削除コマンドで対応
		ARC_DELETE_COMMAND,

		//C-cによりキャンセルされた
		ARC_USER_CANCEL,

		//処理失敗
		ARC_FAILURE,
	};

	std::vector<ArcDll*> m_arcdll_list;

	//B2e.dllは特別扱い
	ArcB2e* m_b2e_dll;

	//対応外のライブラリ(ms/Sでのみ対応)
	ArcDll* m_cal_dll;

	std::vector<Spi*> m_spi_list;

	std::vector<Wcx*> m_wcx_list;

	//分割/結合用一時ディレクトリ
	tstring m_split_temp_dir;

	//処理前のカレントディレクトリ
	tstring m_original_cur_dir;

	//パスワード入力ダイアログのフック
	HMODULE m_hook_dll_module;
	tstring m_hook_dll_path;
	typedef bool(*INSTALLHOOK_PTR)(const DWORD,const unsigned int,const unsigned int);
	typedef bool(*UNINSTALLHOOK_PTR)();
	INSTALLHOOK_PTR m_pInstallHook;
	UNINSTALLHOOK_PTR m_pUninstallHook;

	//プログレスバー
	sslib::ProgressBar* m_progressbar;

	//処理中ファイルの諸々の情報
	struct CUR_FILE{
		tstring arc_path;
		bool auto_renamed;
		tstring recompress_mhd;

		CUR_FILE():arc_path(),auto_renamed(false),recompress_mhd(){}
		CUR_FILE(const tstring& path,bool renamed=false,const tstring& mhd=_T("")):arc_path(path),auto_renamed(renamed),recompress_mhd(mhd){}
	};

	CUR_FILE m_cur_file;

	//cleanup()で削除できる様保存
	tstring m_arc_path;

	///mr@時の圧縮形式受け渡しに利用
	tstring m_recompress_mhd;


	struct ARC_RESULT_MSG{
		ARC_RESULT& result;
		tstring& err_msg;
		ARC_RESULT_MSG(ARC_RESULT& result_,tstring& err_msg_):result(result_),err_msg(err_msg_){}
	};

	struct ARC_THREAD_PARAM{
		Reces* this_ptr;
		std::list<tstring>& file_list;
		tstring file_name;
		ARC_RESULT_MSG& result_msg;

		std::list<tstring> dummy;

		ARC_THREAD_PARAM(Reces* this_ptr_,std::list<tstring>& file_list_,ARC_RESULT_MSG& result_msg_):
			this_ptr(this_ptr_),file_list(file_list_),result_msg(result_msg_){}
		ARC_THREAD_PARAM(Reces* this_ptr_,const TCHAR* file_name_,ARC_RESULT_MSG& result_msg_):
			this_ptr(this_ptr_),file_list(dummy),file_name(file_name_),result_msg(result_msg_){}
		ARC_THREAD_PARAM(Reces* this_ptr_,const tstring& file_name_,ARC_RESULT_MSG& result_msg_):
			this_ptr(this_ptr_),file_list(dummy),file_name(file_name_),result_msg(result_msg_){}
	};

	//書庫処理スレッド
	HANDLE m_arc_thread;
	//動作中ライブラリ
	Archiver* m_arc_dll;

	//クリティカルセクション
	sslib::misc::CriticalSection m_arc_cs,
	m_progressbar_cs,
	m_dialog_hook_cs,
	m_cleanup_cs,m_ctrlc_event_cs;

	sslib::misc::thread::INFO m_progressbar_thread;
	sslib::misc::thread::INFO m_dialog_hook_thread;

private:
	bool info(const TCHAR* msg,...);
	void errmsg(const TCHAR* msg,...);
	tstring getVersion(const TCHAR* file_path);
	//パスワードの入力を求める
	bool requirePassword();
	//入力されたコマンドを実行する
	bool runCommand();
	//ファイルを削除(設定依存)
	bool removeFile(const TCHAR* file_path);
	//m_arcdll_list読み込み
	void loadArcLib();
	//m_arcdll_list解放
	void freeArcLib();
	//ライブラリリストの7-zip32とLMZIP32を入れ替える
	bool swap7ZLMZIP(bool sort_by_name=true);
	//読み込みと対応チェック
	template<typename I>Archiver* loadAndCheck(I ite,I end,const TCHAR* arc_path=NULL,bool* loaded_library=NULL,const TCHAR* ext=NULL,const TCHAR* libname=NULL,const TCHAR* full_libname=NULL);
	//spiやwcxなどプラグインの読み込みと対応チェック
	template<typename T>Archiver* LoadAndCheckPlugin(std::vector<T*>* plugin_list,const TCHAR* arc_path,bool* loaded_library,const tstring& plugin_dir,const TCHAR* libname,Archiver::ARC_TYPE type);
	//'od'と'of'を反映した作成する書庫のパスを作成
	ARC_RESULT arcFileName(CUR_FILE* new_cur_file,const tstring& arc_path,tstring& err_msg);
	//書庫にタイムスタンプをコピー
	bool copyArcTimestamp(const tstring& dest_file_path,FILETIME* source_arc_timestamp);
	//ファイルのフルパスリストを作成
	bool fullPathList(std::list<tstring>& list,std::vector<tstring>& filepaths,bool received=true);

	//*.spiを検索、リストに追加
	bool searchSpi(const TCHAR* search_dir);
	//*.wcxを検索、リストに追加
	bool searchWcx(const TCHAR* search_dir);

	bool parseOptions(sslib::CommandArgument& cmd_arg);

	//プログレスバーを管理
	static unsigned __stdcall manageProgressBar(void* param);

	//パスワードダイアログのフックを処理する
	static unsigned __stdcall dialogHookProc(void* param);

	static unsigned __stdcall callCompress(void* param);
	static unsigned __stdcall callExtract(void* param);
	static unsigned __stdcall callList(void* param);
	static unsigned __stdcall callSendCommands(void* param);
	static unsigned __stdcall callTest(void* param);
	static unsigned __stdcall callSettings(void* param);
	ARC_RESULT compress(std::list<tstring>& compress_file_list,tstring& err_msg);
	ARC_RESULT extract(const tstring& arc_path,tstring& err_msg);
	ARC_RESULT list(const tstring& arc_path,tstring& err_msg);
	ARC_RESULT sendCommands(std::list<tstring>& commands_list,tstring& err_msg);
	ARC_RESULT test(const tstring& arc_path,tstring& err_msg);
	ARC_RESULT settings(tstring& err_msg);

public:
	bool init();
	bool run(sslib::CommandArgument& cmd_arg);
	void cleanup();
	void usage();
	void ctrlCEvent();

};
#endif //_RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681
