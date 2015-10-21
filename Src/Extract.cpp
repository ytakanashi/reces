﻿//Extract.cpp
//解凍

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r30 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"Extract.h"


using namespace sslib;


Extract::ARC_RESULT Extract::operator()(const tstring& arc_path,tstring& err_msg){
	if(IS_TERMINATED)return ARC_USER_CANCEL;

	bool split_file=false;
	tstring join_file_name;

	if(str::locateLastCharacter(arc_path.c_str(),'.')!=-1){
		switch(splitfile::joinFile(arc_path.c_str(),m_split_temp_dir.c_str())){
			case splitfile::join::SUCCESS:{
				split_file=true;
				msg::info(_T("ファイルを結合しました。\n"));
				join_file_name=path::addTailSlash(m_split_temp_dir);
				join_file_name+=path::removeExtension(path::getFileName(arc_path));
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

	//ファイルがロックされていないかテスト
	if(ARCCFG->m_wait){
		bool success=false;
		int retries=0;
		const int max_retries=5;

		do{
			HANDLE handle=::CreateFile(arc_path.c_str(),
									   GENERIC_READ,
									   0,
									   NULL,
									   OPEN_EXISTING,
									   FILE_ATTRIBUTE_NORMAL,
									   NULL);

			if(handle==INVALID_HANDLE_VALUE){
				retries++;
				::Sleep(ARCCFG->m_wait);
				continue;
			}
			success=true;
			SAFE_CLOSE_EX(handle,INVALID_HANDLE_VALUE);
			break;
		}while(retries<max_retries);

		if(!success){
			msg::info(_T("\n"));
			err_msg+=_T("ファイルを開くことが出来ません。");
			err_msg+=arc_path.c_str();
			err_msg+=_T("\n");
			return ARC_FAILURE;
		}
	}

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

		if(!m_arc_dll){
			if(!CFG.general.selected_library_name.empty()){
				//ライブラリが指定されている場合
				m_arc_dll=loadAndCheckPlugin(&m_spi_list,
											 arc_path.c_str(),
											 &loaded_library,
											 CFG.general.spi_dir,
											 CFG.general.selected_library_name.c_str(),
											 Archiver::SPI_AM);
			}else{
				m_arc_dll=loadAndCheck(m_spi_list.begin(),
									   m_spi_list.end(),
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
			}else{
				m_arc_dll=loadAndCheck(m_wcx_list.begin(),
									   m_wcx_list.end(),
									   arc_path.c_str(),
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

				//loadAndCheck()の前に行うこと
				if(!m_b2e_dir.empty()){
					//b2eスクリプトのあるディレクトリを指定
					m_b2e_dll->setScriptDirectory(m_b2e_dir.c_str());
				}

				m_arc_dll=loadAndCheck(v.begin(),
									   v.end(),
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

		if(CFG.mode==MODE_RECOMPRESS){
			//'@'の処理で
			if(CFG.compress.compression_type.c_str()[0]=='@'&&
			   //b2eのformatが指定されておらず
			   CFG.compress.b2e.format.empty()&&
			   //一括再圧縮の一度目か
			   ((!CFG.compress.each_file&&m_cur_file.recompress_mhd.empty())||
				//'/e'
			   CFG.compress.each_file)){
				DWORD mhd_opt=0;

				if(CFG.compress.compression_type.find(_T("pw"))!=tstring::npos){
					mhd_opt|=ArcDll::MHD_PASSWORD;
				}

				if(CFG.compress.compression_type.find(_T("he"))!=tstring::npos){
					mhd_opt|=ArcDll::MHD_HEADERENCRYPTION;
				}

				if(CFG.compress.compression_type.find(_T("sfx"))!=tstring::npos){
					mhd_opt|=ArcDll::MHD_SFX;
				}

				if(m_arc_dll->type()==Archiver::CAL){
					if(m_arc_dll){
						m_cur_file.recompress_mhd.assign(static_cast<ArcDll*>(m_arc_dll)->getCompressionFormat((!split_file)?arc_path.c_str():join_file_name.c_str()));
					}
				}else if(m_arc_dll->type()==Archiver::B2E){
					if(m_b2e_dll){
						unsigned int index=m_b2e_dll->getExtractorIndex((!split_file)?arc_path.c_str():join_file_name.c_str());

						if(index!=0xffffffff){
							tstring buffer(128,'\0');

							m_b2e_dll->getCompressType(index,&buffer,buffer.length());
							m_cur_file.recompress_mhd.assign(buffer);
							CFG.compress.compression_type.clear();
						}
					}
				}

				if(m_cur_file.recompress_mhd.empty()){
					//統合アーカイバ以外
					for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
						if(!(!m_arcdll_list[i]->isLoaded()&&!m_arcdll_list[i]->load())){
							if(m_arcdll_list[i]->isSupportedArchive((!split_file)?arc_path.c_str():join_file_name.c_str())){
								if(!(m_cur_file.recompress_mhd.assign(m_arcdll_list[i]->getCompressionFormat((!split_file)?arc_path.c_str():join_file_name.c_str()))).empty()){
									break;
								}
							}
						}
					}
				}

				if(m_cur_file.recompress_mhd.empty())m_cur_file.recompress_mhd=_T("zip");
				if(mhd_opt&ArcDll::MHD_PASSWORD)m_cur_file.recompress_mhd+=_T("pw");
				if(mhd_opt&ArcDll::MHD_HEADERENCRYPTION)m_cur_file.recompress_mhd+=_T("he");
				if(mhd_opt&ArcDll::MHD_SFX)m_cur_file.recompress_mhd+=_T("sfx");
			}
		}

		if(IS_TERMINATED)return ARC_USER_CANCEL;

		tstring output_dir(path::getParentDirectory(arc_path));
		bool created_dir=false;

		if(!CFG.general.output_dir.empty()){
			std::vector<TCHAR> buffer(MAX_PATHW);

			path::getFullPath(&buffer[0],
							  buffer.size(),
							  CFG.general.output_dir.c_str(),
							  ((!CFG.general.default_base_dir)?
							   output_dir.c_str():
							   m_original_cur_dir.c_str()));
			output_dir.assign(&buffer[0]);
			if(!path::fileExists(output_dir.c_str())){
				if(!fileoperation::createDirectory(output_dir.c_str())){
					err_msg.assign(format(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str()));
					return ARC_CANNOT_CREATE_DIRECTORY;
				}
			}
		}

		if(CFG.mode==MODE_RECOMPRESS){
			output_dir=ARCCFG->m_recmp_temp_dir.c_str();
		}

		//新規ディレクトリ作成
		if(CFG.extract.create_dir){
			bool redundant_dir=false;

			//ディレクトリ階層を無視する場合は二重ディレクトリを考慮しない
			if(CFG.general.ignore_directory_structures){
				CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=false;
			}

			//パスを二つ以上除く場合であれば二重ディレクトリ防止を無効に
			if(CFG.compress.exclude_base_dir==-1||
			   CFG.compress.exclude_base_dir){
				CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir=false;
			}

			tstring root_dir;

			if(CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir||
			   CFG.extract.create_dir_optimization.remove_redundant_dir.only_file){
				redundant_dir=
					m_arc_dll->isRedundantDir((!split_file)?arc_path.c_str():join_file_name.c_str(),
														 CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir,
														 CFG.extract.create_dir_optimization.remove_redundant_dir.only_file,
														 &root_dir);
			}

			{
				//新規ディレクトリ名作成
				tstring new_dir_name(path::getFileName((!split_file)?arc_path:join_file_name));

				//tar系を考慮しつつ拡張子削除
				new_dir_name=removeExtensionEx(new_dir_name);

				//同名の二重ディレクトリを防ぐ
				if(redundant_dir&&
				   CFG.extract.create_dir_optimization.remove_redundant_dir.same_dir&&
				   new_dir_name!=root_dir){
					redundant_dir=false;
				}

				if(!redundant_dir){
					if(CFG.extract.create_dir_optimization.omit_number_and_symbol.number&&
					   CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol){
						//ディレクトリ名末尾の数字と記号を削除
						new_dir_name=path::removeLastNumbersAndSymbols(new_dir_name);
					}else if(CFG.extract.create_dir_optimization.omit_number_and_symbol.number){
						//ディレクトリ名末尾の数字を削除
						new_dir_name=path::removeLastNumbers(new_dir_name);
					}else if(CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol){
						//ディレクトリ名末尾の記号を削除
						new_dir_name=path::removeLastSymbols(new_dir_name);
					}

					//ディレクトリ名末尾のスペースとドットを削除
					new_dir_name=path::removeSpacesAndDots(new_dir_name);

					output_dir=path::addTailSlash(output_dir)+=new_dir_name;

					created_dir=fileoperation::createDirectory(output_dir.c_str());
					if(!created_dir){
						err_msg=format(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str());
						return ARC_CANNOT_CREATE_DIRECTORY;
					}
				}
			}
		}

		//区切り文字を末尾に追加
		output_dir=path::addTailSlash(output_dir);

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_CREATE_PROGRESSBAR);
		}
		//コールバック関数を登録
		m_arc_dll->setCallback(m_progressbar_thread.id);

		if(IS_TERMINATED)return ARC_USER_CANCEL;

		//解凍
		tstring log;
		bool result=false;

		switch(m_arc_dll->extract((!split_file)?arc_path.c_str():join_file_name.c_str(),
											 output_dir.c_str(),
											 &log)){
			case Archiver::ARC_SUCCESS:
				result=true;
				break;
			case Archiver::ARC_CANNOT_OPEN_LISTFILE:
				msg::err(_T("リストファイルを開くことが出来ませんでした。\n"));
				break;
			case Archiver::ARC_NO_MATCHES_FOUND:
				msg::err(_T("フィルタに一致するファイルはありません。\n"));
				break;
			case Archiver::ARC_FAILURE:
			default:
				err_msg=_T("エラーが発生しました。\n");
				break;
		}

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_DESTROY_PROGRESSBAR);
		}
		//コールバック関数の登録を解除
		m_arc_dll->clearCallback();

		if(created_dir&&
		   CFG.extract.create_dir_optimization.copy_timestamp){
			//作成したディレクトリの更新日時を書庫と同じにする
			File arc(arc_path.c_str(),
						  OPEN_EXISTING,
						  GENERIC_READ,
						  FILE_SHARE_READ);
			File dir(output_dir.c_str());
			FILETIME ft={};

			arc.getFileTime(&ft);
			dir.setFileTime(&ft);
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
