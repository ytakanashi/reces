//Extract.cpp
//解凍

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"Extract.h"


using namespace sslib;


Extract::ARC_RESULT Extract::operator()(const tstring& arc_path,tstring& err_msg){
	if(isTerminated())return ARC_USER_CANCEL;

	bool split_file=false;
	tstring join_file_name;

	if(str::locateLastCharacter(arc_path.c_str(),'.')!=-1){
		switch(fileoperation::joinFile(arc_path.c_str(),m_split_temp_dir.c_str())){
			case fileoperation::JFRET_SUCCESS:{
				split_file=true;
				info(_T("ファイルを結合しました。\n"));
				join_file_name=path::addTailSlash(m_split_temp_dir);
				join_file_name+=path::removeExtension(path::getFileName(arc_path));
				break;
			}
			case fileoperation::JFRET_NOT_SPLIT:
			default:
				//分割された書庫ではない
				break;
			case fileoperation::JFRET_CANNOT_CREATE:
			case fileoperation::JFRET_MALLOC_ERR:
				err_msg=_T("ファイルの結合に失敗しました。\n");
			return ARC_FAILURE;
		}
	}

	m_arc_dll=NULL;

	info(_T("ライブラリを読み込んでいます..."));

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
			info(_T("\n"));
			err_msg+=_T("ファイルを開くことが出来ません。");
			err_msg+=arc_path.c_str();
			err_msg+=_T("\n");
			return ARC_FAILURE;
		}
	}

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
				return ARC_NOT_SUPPORTED_METHOD;
			}
		}
	}

	if(isTerminated())return ARC_USER_CANCEL;

	info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if(m_arc_dll->type()==Archiver::CAL&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			info(_T("バックグラウンドモードに設定しました。\n"));
		}

		if(CFG.mode==MODE_RECOMPRESS){
			//'@'の処理で
			if(CFG.compress.compression_type.c_str()[0]=='@'&&
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
						m_cur_file.recompress_mhd.assign(static_cast<ArcDll*>(m_arc_dll)->getCompressionMethod((!split_file)?arc_path.c_str():join_file_name.c_str()));
					}
				}else{
					//統合アーカイバ以外
					for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
						if(!(!m_arcdll_list[i]->isLoaded()&&!m_arcdll_list[i]->load())){
							if(m_arcdll_list[i]->isSupportedArchive((!split_file)?arc_path.c_str():join_file_name.c_str())){
								if(!(m_cur_file.recompress_mhd.assign(m_arcdll_list[i]->getCompressionMethod((!split_file)?arc_path.c_str():join_file_name.c_str()))).empty()){
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

		if(isTerminated())return ARC_USER_CANCEL;

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

		if(m_arc_dll->type()==Archiver::CAL&&
		   CFG.mode==MODE_RECOMPRESS&&
		   CFG.recompress.run_command.disable()&&
		   CFG.compress.each_file&&
		   !CFG.general.ignore_directory_structures&&
		   CFG.general.custom_param.empty()&&
		   CFG.compress.exclude_base_dir==0&&
		   !CFG.extract.create_dir&&
		   !CFG.general.decode_uesc){
			//可能であれば解凍->圧縮ではなく、ライブラリの削除コマンドを使用して処理

			tstring current_mhd=static_cast<ArcDll*>(m_arc_dll)->getCompressionMethod((!split_file)?arc_path.c_str():join_file_name.c_str());

			if(!current_mhd.empty()){
				if(str::isEqualStringIgnoreCase((CFG.compress.compression_type.c_str()[0]!='@')?
												CFG.compress.compression_type:
												m_cur_file.recompress_mhd,
												current_mhd)){

					tstring old_arc_path((!split_file)?arc_path:join_file_name);
					CUR_FILE new_cur_file(old_arc_path);

					if(str::isEqualStringIgnoreCase(path::getExtension(old_arc_path),_T("exe"))){
						//拡張子がexeなら削る
						new_cur_file.arc_path=old_arc_path=removeExtensionEx(old_arc_path);
					}

					tstring ext=getExtensionEx(old_arc_path);
					tstring old_arc_path_bak(old_arc_path);
					bool need_ext=false;

					if(CFG.general.auto_rename&&
					   !ext.empty()&&
					   old_arc_path.find(str::toLower(ext))!=tstring::npos){
						ext=_T(".")+getExtensionEx(old_arc_path);
						old_arc_path=removeExtensionEx(old_arc_path);
						need_ext=true;
					}

					if(arcFileName(&new_cur_file,old_arc_path,err_msg)==ARC_CANNOT_CREATE_DIRECTORY){
						return ARC_CANNOT_CREATE_DIRECTORY;
					}

					if(CFG.general.auto_rename&&
					   need_ext){
						new_cur_file.arc_path=removeExtensionEx(new_cur_file.arc_path);
						new_cur_file.arc_path+=ext;
					}

					//出力するファイルのパスが同一か、異なる場合はそのファイルが存在しない場合のみ処理
					if(old_arc_path==new_cur_file.arc_path||
					   (!path::fileExists(new_cur_file.arc_path.c_str())&&old_arc_path!=new_cur_file.arc_path)){
						//オリジナルとパスが異なるようであればコピー
						bool copy_file=!split_file&&
							old_arc_path!=new_cur_file.arc_path;

						if(copy_file){
							::CopyFile(arc_path.c_str(),
									   new_cur_file.arc_path.c_str(),false);
						}else if(split_file){
							::MoveFileEx(join_file_name.c_str(),
										 new_cur_file.arc_path.c_str(),
										 MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
						}

						tstring log;

						if(static_cast<ArcDll*>(m_arc_dll)->del(new_cur_file.arc_path.c_str(),&log)==Archiver::ARC_SUCCESS){
							if(!CFG.no_display.no_log){
								STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());
							}

							m_cur_file=new_cur_file;

							return ARC_DELETE_COMMAND;
						}else{
							if(copy_file){
								//失敗したため削除
								::DeleteFile(new_cur_file.arc_path.c_str());
							}else if(split_file){
								//ファイルを元の位置に戻す
								::MoveFileEx(new_cur_file.arc_path.c_str(),join_file_name.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
							}
						}
					}
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

			if(CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir||
			   CFG.extract.create_dir_optimization.remove_redundant_dir.only_file){
				redundant_dir=
					m_arc_dll->isRedundantDir((!split_file)?arc_path.c_str():join_file_name.c_str(),
														 CFG.extract.create_dir_optimization.remove_redundant_dir.double_dir,
														 CFG.extract.create_dir_optimization.remove_redundant_dir.only_file);
			}

			if(!redundant_dir){
				//新規ディレクトリ名作成
				tstring new_dir_name(path::getFileName((!split_file)?arc_path:join_file_name));

				//tar系を考慮しつつ拡張子削除
				new_dir_name=removeExtensionEx(new_dir_name);

				if(CFG.extract.create_dir_optimization.omit_number_and_symbol.number&&
				   CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol){
					//ディレクトリ名末尾の数字と記号を削除
					new_dir_name=path::removeLastNumberAndSymbol(new_dir_name);
				}else if(CFG.extract.create_dir_optimization.omit_number_and_symbol.number){
					//ディレクトリ名末尾の数字を削除
					new_dir_name=path::removeLastNumber(new_dir_name);
				}else if(CFG.extract.create_dir_optimization.omit_number_and_symbol.symbol){
					//ディレクトリ名末尾の記号を削除
					new_dir_name=path::removeLastSymbol(new_dir_name);
				}

				output_dir=path::addTailSlash(output_dir)+=new_dir_name;

				created_dir=fileoperation::createDirectory(output_dir.c_str());
				if(!created_dir){
					//コールバック関数の登録を解除
					m_arc_dll->clearCallback();
					err_msg=format(_T("ディレクトリ '%s' の作成に失敗しました。\n"),output_dir.c_str());
					return ARC_CANNOT_CREATE_DIRECTORY;
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

		if(isTerminated())return ARC_USER_CANCEL;

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
				errmsg(_T("リストファイルを開くことが出来ませんでした。\n"));
				break;
			case Archiver::ARC_NO_MATCHES_FOUND:
				errmsg(_T("フィルタに一致するファイルはありません。\n"));
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
			void* msg_buffer=NULL;

			::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
							FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							::GetLastError(),
							MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT),
							static_cast<TCHAR*>(msg_buffer),
							0,
							NULL
							);
			if(msg_buffer!=NULL)errmsg(_T("%s\n"),static_cast<TCHAR*>(msg_buffer));
			::LocalFree(msg_buffer);
		}

		if(!CFG.no_display.no_log){
			STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());
		}
		return (result)?ARC_SUCCESS:ARC_FAILURE;
	}
	return ARC_FAILURE;
}
