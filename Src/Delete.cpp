//Delete.cpp
//削除

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r30 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"Delete.h"


using namespace sslib;


Delete::ARC_RESULT Delete::operator()(const tstring& arc_path,tstring& err_msg){
	if(IS_TERMINATED)return ARC_USER_CANCEL;

	m_arc_dll=NULL;

	msg::info(_T("ライブラリを読み込んでいます..."));

	loadArcLib();

	{
		bool loaded_library=false;

		tstring current_dir(path::getCurrentDirectory());

		::SetCurrentDirectory(m_original_cur_dir.c_str());

		if(!CFG.general.selected_library_name.empty()){
			//ライブラリが指定されている場合
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str());
		}else{
			//拡張子からの推測
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   path::getExtension(arc_path).c_str());

			//総当たり
			if(!m_arc_dll){
				m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
									   m_arcdll_list.end(),
									   arc_path.c_str(),
									   &loaded_library);
			}
		}

		::SetCurrentDirectory(current_dir.c_str());

		if(!m_arc_dll){
			if(ARCCFG->m_password_input_cancelled){
				STDOUT.outputString(_T("\n"));
				err_msg=_T("パスワードの入力がキャンセルされました。\n");
				return ARC_INPUT_PASSWORD_CANCEL;
			}else if(!CFG.general.selected_library_name.empty()&&!loaded_library){
				STDOUT.outputString(_T("\n"));
				err_msg=format(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),CFG.general.selected_library_name.c_str());
				return ARC_CANNOT_LOAD_LIBRARY;
			}else{
				STDOUT.outputString(_T("\n"));
				err_msg=_T("対応していない圧縮形式かファイルが壊れています。\n");
				return ARC_NOT_SUPPORTED_FORMAT;
			}
		}
	}

	if(IS_TERMINATED)return ARC_USER_CANCEL;

	msg::info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			msg::info(_T("バックグラウンドモードに設定しました。\n"));
		}

		if(IS_TERMINATED)return ARC_USER_CANCEL;

		//削除
		tstring log;
		bool result=false;

		switch(static_cast<ArcDll*>(m_arc_dll)->del(arc_path.c_str(),&log)){
			case Archiver::ARC_SUCCESS:
				result=true;
				break;
			case Archiver::ARC_CANNOT_OPEN_LISTFILE:
				msg::err(_T("リストファイルを開くことが出来ませんでした。\n"));
				break;
			case Archiver::ARC_NO_MATCHES_FOUND:
				msg::err(_T("フィルタに一致するファイルはありません。\n"));
				break;
			case Archiver::ARC_NOT_IMPLEMENTED:
				msg::err(_T("実装されていない機能です。\n"));
				break;
			case Archiver::ARC_NO_FILTER:
				msg::err(_T("フィルタが指定されていません。\n"));
				break;
			case Archiver::ARC_FAILURE:
			default:
				err_msg=_T("エラーが発生しました。\n");
				break;
		}

		if(!result){
			msg::lasterr();
		}

		if(!CFG.no_display.no_log){
			STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());
		}
		return (result)?ARC_SUCCESS:ARC_FAILURE;
	}
	return ARC_FAILURE;
}
