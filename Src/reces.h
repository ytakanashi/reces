//reces.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r31 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681
#define _RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681


#include"recesBase.h"


class Reces:public sslib::ConsoleApp,public RecesBase{
public:
	Reces():
		m_hook_dll_schedule_del(),
		m_hook_dll(),
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
	sslib::fileoperation::scheduleDelete m_hook_dll_schedule_del;
	sslib::Library m_hook_dll;
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

	class ArchiverThread{
		public:
			template<typename T>ArchiverThread(Reces* this_ptr_,T& data):
				result(ARC_FAILURE),
				err_msg(),
				result_msg(result,err_msg),
				thread_param(this_ptr_,data,result_msg),
				this_ptr(this_ptr_){}

			template<typename T>ARC_RESULT run(){
				this_ptr->m_arc_thread=(HANDLE)_beginthreadex(NULL,0,&Reces::processArchive<T>,&thread_param,0,NULL);

				::WaitForSingleObject(this_ptr->m_arc_thread,INFINITE);
				{
					sslib::misc::Lock lock(this_ptr->m_arc_cs);
					SAFE_CLOSE(this_ptr->m_arc_thread);
				}
				return result;
			}

			void err(){
				msg::err(err_msg.c_str());
				this_ptr->setExitCode(EXIT_FAILURE);
			}

		private:
			ARC_RESULT result;
			tstring err_msg;
			ARC_RESULT_MSG result_msg;
			ARC_THREAD_PARAM thread_param;
			Reces* this_ptr;
	};

	//クリティカルセクション
	sslib::misc::CriticalSection m_arc_cs,m_dialog_hook_cs,
	m_cleanup_cs,m_ctrlc_event_cs,m_progressbar_cs;

	sslib::misc::thread::INFO m_dialog_hook_thread;

private:
	//パスワードの入力を求める
	bool requirePassword();
	//入力されたコマンドを実行する
	bool runCommand();
	//ファイルを削除(設定依存)
	bool removeFile(const TCHAR* file_path);

	//*.spiを検索、リストに追加
	bool searchSpi(const TCHAR* search_dir);
	//*.wcxを検索、リストに追加
	bool searchWcx(const TCHAR* search_dir);

	bool recompress(std::list<tstring>& file_list);
	bool compress(std::list<tstring>& file_list);
	template<typename T>bool extract(std::list<tstring>& file_list);
	bool list(std::list<tstring>& file_list);
	bool test(std::list<tstring>& file_list);
	bool sendCommands(std::vector<tstring>& filepaths);
	bool version(std::vector<tstring>& filepaths);
	bool settings();

	//プログレスバーを管理
	static unsigned __stdcall manageProgressBar(void* param);

	//パスワードダイアログのフックを処理する
	static unsigned __stdcall dialogHookProc(void* param);

	struct getParamData{
		//Extract,List,Test
		operator tstring&(){return param->file_name;}
		//Compress,SendCommands
		operator std::list<tstring>&(){return param->file_list;}
		//Settings
		operator HWND(){return param->this_ptr->wnd();}
		ARC_THREAD_PARAM* param;
		getParamData(ARC_THREAD_PARAM* param_):param(param_){}
	};

	template<typename T>static unsigned __stdcall processArchive(void* param);

public:
	bool init();
	bool run(sslib::CommandArgument& cmd_arg);
	void cleanup();
	void usage();
	void ctrlCEvent();

};
#endif //_RECES_H_2B802DBE_E1B2_48c1_B8C1_A1C87CBBF681
