//ArcCfg.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r21 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _ARCCFG_H_78EA4B91_D18C_449e_8947_3D2048E1188C
#define _ARCCFG_H_78EA4B91_D18C_449e_8947_3D2048E1188C

#include"PrivateProfile.h"



class ArcCfg{
public:
	ArcCfg():
		m_config(),
		m_recmp_temp_dir(),
		m_list_temp_dir(),
		m_password_input_cancelled(false),
		m_hook_dialog_type(0),
		m_extracting_wnd_handle(NULL){}
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
	//ArcDll側にsetCallbackProc()で渡す
	HWND m_extracting_wnd_handle;

public:
	inline CONFIG& cfg(){return m_config.cfg();}
};
#endif //_ARCCFG_H_78EA4B91_D18C_449e_8947_3D2048E1188C
