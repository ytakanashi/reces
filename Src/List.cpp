//List.cpp
//リスト

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r34 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"List.h"


using namespace sslib;


List::ARC_RESULT List::operator()(const tstring& arc_path,tstring& err_msg){
	if(IS_TERMINATED)return ARC_USER_CANCEL;

	bool joined_file=false;
	tstring joined_file_name;

	m_arc_dll=NULL;

	msg::info(_T("ライブラリを読み込んでいます..."));

	loadArcLib();

	{
		bool loaded_library=false;

		if(!CFG.general.selected_library_name.empty()){
			//ライブラリが指定されている場合
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   NULL,
								   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
								   CFG.general.selected_library_name.c_str(),
								   CFG.general.dll_dir);
		}else{
			//拡張子からの推測
			m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
								   m_arcdll_list.end(),
								   arc_path.c_str(),
								   &loaded_library,
								   path::getExtension(arc_path).c_str(),
								   NULL,NULL,
								   CFG.general.dll_dir);

			//総当たり
			if(!m_arc_dll){
				m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
									   m_arcdll_list.end(),
									   arc_path.c_str(),
									   &loaded_library,
									   NULL,NULL,NULL,
									   CFG.general.dll_dir);
			}
		}

		if(m_arc_dll&&str::isEqualStringIgnoreCase(m_arc_dll->name(),_T("tar32"))||
		   !m_arc_dll){
			//tar系は分割書庫であっても判定可能
			if(str::locateLastCharacter(arc_path.c_str(),'.')!=-1){
				switch(splitfile::joinFile(arc_path.c_str(),m_split_temp_dir.c_str())){
					case splitfile::join::SUCCESS:{
						dprintf(_T("splitfile::join::SUCCESS\n"));
						joined_file=true;
						joined_file_name=path::addTailSlash(m_split_temp_dir.c_str());
						joined_file_name+=removeExtensionEx(path::getFileName(arc_path));
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

			if(!m_arc_dll){
				//結合ファイルで再確認
				if(!CFG.general.selected_library_name.empty()){
					//ライブラリが指定されている場合
					m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
										   m_arcdll_list.end(),
										   (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
										   &loaded_library,
										   NULL,
										   path::removeExtension(path::getFileName(CFG.general.selected_library_name)).c_str(),
										   CFG.general.selected_library_name.c_str(),
										   CFG.general.dll_dir);
				}else{
					//拡張子からの推測
					m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
										   m_arcdll_list.end(),
										   (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
										   &loaded_library,
										   path::getExtension(arc_path).c_str(),
										   NULL,NULL,
										   CFG.general.dll_dir);

					//総当たり
					if(!m_arc_dll){
						m_arc_dll=loadAndCheck(m_arcdll_list.begin(),
											   m_arcdll_list.end(),
											   (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
											   &loaded_library,
											   NULL,NULL,NULL,
											   CFG.general.dll_dir);
					}
				}
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=loadAndCheckPlugin(&m_spi_list,
											 (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
											 &loaded_library,
											 CFG.general.spi_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::SPI_AM);
			}else{
				m_arc_dll=loadAndCheck(m_spi_list.begin(),
									   m_spi_list.end(),
									   (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=loadAndCheckPlugin(&m_wcx_list,
											 (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
											 &loaded_library,
											 CFG.general.wcx_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::WCX);
			}else{
				m_arc_dll=loadAndCheck(m_wcx_list.begin(),
									   m_wcx_list.end(),
									   (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
									   &loaded_library);
			}
		}

		if(!m_arc_dll&&m_b2e_dll){
			if(!CFG.general.selected_library_name.empty()&&
			   str::isEqualStringIgnoreCase(path::removeExtension(path::getFileName(CFG.general.selected_library_name)),
											m_b2e_dll->name())||
			   CFG.general.selected_library_name.empty()){

				std::vector<Archiver*> v;

				v.push_back(m_b2e_dll);

				if(!m_b2e_dir.empty()){
					//b2eスクリプトのあるディレクトリを指定
					m_b2e_dll->setScriptDirectory(m_b2e_dir.c_str());
				}

				m_arc_dll=loadAndCheck(v.begin(),
									   v.end(),
									   (!joined_file)?arc_path.c_str():joined_file_name.c_str(),
									   &loaded_library);
			}
		}

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

		//リスト出力
		switch(m_arc_dll->list((!joined_file)?arc_path.c_str():joined_file_name.c_str())){
			case Archiver::ARC_SUCCESS:
				return ARC_SUCCESS;
			case Archiver::ARC_FAILURE:
			default:
				return ARC_FAILURE;
		}
	}
	return ARC_SUCCESS;
}
