//ArcCfg.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r31 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCCFG_H_78EA4B91_D18C_449e_8947_3D2048E1188C
#define _ARCCFG_H_78EA4B91_D18C_449e_8947_3D2048E1188C

#include"PrivateProfile.h"


#define CFG ArcCfg::inst()->cfg()
#define ARCCFG ArcCfg::inst()

class ArcCfg:private sslib::misc::NonCopyable<ArcCfg>{
public:
	ArcCfg():
		m_config(),
		m_recmp_temp_dir(),
		m_list_temp_dir(),
		m_schedule_list(),
		m_password_input_cancelled(false),
		m_hook_dialog_type(0),
		m_extracting_wnd_handle(NULL),
		m_wait(0){}
	~ArcCfg(){}

private:
	Config m_config;

public:
	//再圧縮用一時ディレクトリ
	tstring m_recmp_temp_dir;
	//リストファイル出力用一時ディレクトリ
	tstring m_list_temp_dir;

	//終了時に削除するディレクトリ/ファイルのリスト
	std::vector<sslib::fileoperation::scheduleDelete*> m_schedule_list;

	//パスワードの入力がEnter押下によりキャンセルされた
	bool m_password_input_cancelled;
	//フック出来たダイアログの種類
	int m_hook_dialog_type;

	//進捗ダイアログのハンドル
	//今のところXacrett.dllの為のみに用意
	//ArcDll側にsetCallback()で渡す
	HWND m_extracting_wnd_handle;

	//隠しオプション
	//解凍時のみ有効
	int m_wait;

public:
	static ArcCfg* inst(){
		static ArcCfg inst;
		return &inst;
	}
	inline CONFIG& cfg(){return m_config.cfg();}
};
#endif //_ARCCFG_H_78EA4B91_D18C_449e_8947_3D2048E1188C
