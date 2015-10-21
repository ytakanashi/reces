//Test.cpp
//テスト

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r30 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"Test.h"


using namespace sslib;


Test::ARC_RESULT Test::operator()(const tstring& arc_path,tstring& err_msg){
	if(IS_TERMINATED)return ARC_USER_CANCEL;

	bool split_file=false;
	tstring join_file_name;

	if(str::locateLastCharacter(arc_path.c_str(),'.')!=-1){
		switch(splitfile::joinFile(arc_path.c_str(),m_split_temp_dir.c_str())){
			case splitfile::join::SUCCESS:{
				split_file=true;
				STDOUT.outputString(_T("ファイルを結合しました。\n"));
				join_file_name=path::addTailSlash(m_split_temp_dir.c_str());
				join_file_name+=removeExtensionEx(path::getFileName(arc_path));
				break;
			}
			case splitfile::join::NOT_SPLIT:
			default:
				//分割された書庫ではない
				break;
			case splitfile::join::CANNOT_CREATE:
			case splitfile::join::MALLOC_ERR:
				err_msg=_T("ファイルの結合に失敗しました。\n");
				return ARC_FAILURE;
		}
	}

	m_arc_dll=NULL;

	msg::info(_T("ライブラリを読み込んでいます..."));

	loadArcLib();

	{
		bool loaded_library=false;

		if(!CFG.general.selected_library_name.empty()){
			//ライブラリが指定されている場合
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   NULL,
								   NULL,
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

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=loadAndCheckPlugin(&m_wcx_list,
											 arc_path.c_str(),
											 &loaded_library,
											 CFG.general.wcx_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::WCX);
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()&&!loaded_library){
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

		//リスト出力
		switch(m_arc_dll->test((!split_file)?arc_path.c_str():join_file_name.c_str())){
			case Archiver::ARC_SUCCESS:
				return ARC_SUCCESS;
			case Archiver::ARC_FAILURE:
			default:
				return ARC_FAILURE;
		}
	}
	return ARC_FAILURE;
}
