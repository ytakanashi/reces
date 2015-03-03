//reces.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681
#define _RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681

#include"recesBase.h"


class Reces:public sslib::ConsoleApp,public RecesBase{
public:
	Reces():
		m_hook_dll_module(NULL),
		m_hook_dll_path(),
		m_pInstallHook(NULL),
		m_pUninstallHook(NULL),
		m_progressbar(NULL),
		m_arc_thread(NULL),
		m_arc_cs(),
		m_dialog_hook_cs(),
		m_cleanup_cs(),
		m_ctrlc_event_cs(),
		m_progressbar_cs(),
		m_dialog_hook_thread(){}
	~Reces(){}

private:
	//パスワード入力ダイアログのフック
	HMODULE m_hook_dll_module;
	tstring m_hook_dll_path;
	typedef bool(*INSTALLHOOK_PTR)(const DWORD,const unsigned int,const unsigned int);
	typedef bool(*UNINSTALLHOOK_PTR)();
	INSTALLHOOK_PTR m_pInstallHook;
	UNINSTALLHOOK_PTR m_pUninstallHook;

	//プログレスバー
	sslib::ProgressBar* m_progressbar;

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

	//クリティカルセクション
	sslib::misc::CriticalSection m_arc_cs,m_dialog_hook_cs,
	m_cleanup_cs,m_ctrlc_event_cs,m_progressbar_cs;

	sslib::misc::thread::INFO m_dialog_hook_thread;

private:
	tstring getVersion(const TCHAR* file_path);
	//パスワードの入力を求める
	bool requirePassword();
	//入力されたコマンドを実行する
	bool runCommand();
	//ファイルを削除(設定依存)
	bool removeFile(const TCHAR* file_path);
	//書庫にタイムスタンプをコピー
	bool copyArcTimestamp(const tstring& dest_file_path,FILETIME* source_arc_timestamp);

	//*.spiを検索、リストに追加
	bool searchSpi(const TCHAR* search_dir);
	//*.wcxを検索、リストに追加
	bool searchWcx(const TCHAR* search_dir);

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

public:
	bool init();
	bool run(sslib::CommandArgument& cmd_arg);
	void cleanup();
	void usage();
	void ctrlCEvent();

};
#endif //_RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681
