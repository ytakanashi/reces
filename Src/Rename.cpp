//Rename.cpp
//リネーム

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r32 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"Rename.h"


using namespace sslib;


bool Rename::addPattern(){
	Console input_pattern(STD_INPUT_HANDLE);
	bool result=false;

	STDOUT.outputString(_T("リネーム条件入力"));
	STDOUT.outputString((CFG.rename.regex)?_T("(正規表現): \n"):_T(": \n"));

	std::vector<TCHAR> pattern_buffer(1024,'\0');
	std::vector<TCHAR> replacement_buffer(1024,'\0');
	tstring pattern;
	tstring replacement;
	DWORD read=0;

	do{
		pattern_buffer.assign(pattern_buffer.size(),'\0');
		pattern.clear();
		replacement_buffer.assign(replacement_buffer.size(),'\0');
		replacement.clear();

		STDOUT.outputString(_T("検索文字列: "));
		input_pattern.read(&pattern_buffer[0],pattern_buffer.size(),&read);
		pattern.assign(&pattern_buffer[0]);
		//空文字Enter
		if(pattern.find_first_of(_T("\r\n"))==0)break;
		str::chomp(pattern);
		if(strvalid(pattern.c_str())){
			STDOUT.outputString(_T("置換後文字列: "));
			input_pattern.read(&replacement_buffer[0],replacement_buffer.size(),&read);
			replacement.assign(&replacement_buffer[0]);
			str::chomp(replacement);
			CFG.rename.pattern_list.push_back(RENAME::pattern(pattern,replacement));
		}
	}while(!IS_TERMINATED&&
		   read);

	return !CFG.rename.pattern_list.empty();
}

Rename::ARC_RESULT Rename::operator()(const tstring& arc_path,tstring& err_msg){
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

		if(CFG.rename.pattern_list.empty()&&
		   !addPattern()){
			msg::err(_T("フィルタが指定されていません。\n"));
			return ARC_FAILURE;
		}

		if(IS_TERMINATED)return ARC_USER_CANCEL;

		//リネーム
		tstring log;
		bool result=false;

		switch(static_cast<ArcDll*>(m_arc_dll)->rename(arc_path.c_str(),&log)){
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
