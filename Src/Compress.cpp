//Compress.cpp
//圧縮

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r33 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"Stdafx.h"
#include"Compress.h"


using namespace sslib;



Compress::ARC_RESULT Compress::operator()(std::list<tstring>& compress_file_list,tstring& err_msg){
	if(IS_TERMINATED)return ARC_USER_CANCEL;

	m_arc_dll=NULL;

	msg::info(_T("ライブラリを読み込んでいます..."));

	if(CFG.compress.b2e.format.empty()){
		for(size_t i=0,list_size=m_arcdll_list.size();i<list_size;i++){
			if(m_arcdll_list[i]->isSupportedFormat((CFG.compress.compression_type.c_str()[0]!='@')?
												   CFG.compress.compression_type.c_str():
												   m_cur_file.recompress_mhd.c_str())){
				if(m_arcdll_list[i]->load())m_arc_dll=m_arcdll_list[i];
				break;
			}
		}
	}

	if(!m_arc_dll&&m_b2e_dll){
		m_b2e_dll->load();
		if(!m_b2e_dir.empty()){
			//b2eスクリプトのあるディレクトリを指定
			m_b2e_dll->setScriptDirectory(m_b2e_dir.c_str());
		}
		//圧縮用スクリプトを列挙
		m_b2e_dll->enumCompressScript();
		if(m_b2e_dll->isSupportedFormat((m_cur_file.recompress_mhd.empty())?
											   CFG.compress.b2e.format.c_str():
											   m_cur_file.recompress_mhd.c_str(),
											   CFG.compress.b2e.method.c_str())){
			m_arc_dll=m_b2e_dll;
		}
	}

	if(!m_arc_dll){
		STDOUT.outputString(_T("\n"));
		err_msg=_T("対応していない圧縮形式かファイルが壊れています。\n");
		return ARC_NOT_SUPPORTED_FORMAT;
	}

	if(m_arc_dll->type()==Archiver::CAL){
		if((CFG.mode==MODE_RECOMPRESS||CFG.mode==MODE_COMPRESS)&&
		   static_cast<ArcDll*>(m_arc_dll)->getFormat().opt&ArcDll::MHD_SFX&&
		   !CFG.compress.split_value.empty()){
			err_msg=_T("オプションの組み合わせが正しくありません。(sfx+/s)\n");
			return ARC_INCORRECT_OPTION_SFX_S;
		}
	}

	if(IS_TERMINATED)return ARC_USER_CANCEL;

	if(!m_arc_dll->isLoaded()&&!m_arc_dll->load()){
		STDOUT.outputString(_T("\n"));
		err_msg=format(_T("ライブラリ '%s' の読み込みに失敗しました。\n"),m_arc_dll->name().c_str());
		return ARC_CANNOT_LOAD_LIBRARY;
	}

	msg::info(_T(" %s\n"),m_arc_dll->getInformation().c_str());

	if((m_arc_dll->type()==Archiver::CAL||
		m_arc_dll->type()==Archiver::B2E)&&
	   static_cast<ArcDll*>(m_arc_dll)->getRunning()){
		err_msg=_T("他のプロセスで使用中です。\n");
		return ARC_LIBRARY_IS_BUSY;
	}else{
		if(CFG.general.background_mode&&
		   m_arc_dll->setBackgroundMode(true)){
			msg::info(_T("バックグラウンドモードに設定しました。\n"));
		}

		if(IS_TERMINATED)return ARC_USER_CANCEL;

		{
			//'od'と'of'を反映した作成する書庫のパスを作成
			if(updateArcFileName(&m_cur_file,m_cur_file.arc_path,err_msg)==ARC_CANNOT_CREATE_DIRECTORY){
				return ARC_CANNOT_CREATE_DIRECTORY;
			}

			if(path::isDirectory(m_cur_file.arc_path.c_str())&&
			   CFG.compress.raw_file_name){
				//作成する書庫と同名のディレクトリが存在する場合oFを無効にして再度パス作成
				CFG.compress.raw_file_name=false;
				if(updateArcFileName(&m_cur_file,m_cur_file.arc_path,err_msg)==ARC_CANNOT_CREATE_DIRECTORY){
					return ARC_CANNOT_CREATE_DIRECTORY;
				}
			}
		}

		//本来の出力先ディレクトリ
		tstring orig_dir;
		bool split_file=!CFG.compress.split_value.empty();

		if(split_file){
			orig_dir=path::getParentDirectory(m_cur_file.arc_path);
			m_cur_file.arc_path=path::addTailSlash(m_split_temp_dir)+path::getFileName(m_cur_file.arc_path);
		}

		if(IS_TERMINATED)return ARC_USER_CANCEL;

		if(CFG.general.ignore_directory_structures){
			//ディレクトリ階層を無視する場合/ebを無効に
			CFG.compress.exclude_base_dir=0;
		}

		if(!STDOUT.isRedirected()&&
		   !CFG.no_display.no_information){
			misc::thread::post(m_progressbar_thread.id,WM_CREATE_PROGRESSBAR);
		}
		//コールバック関数を登録
		m_arc_dll->setCallback(m_progressbar_thread.id);

		if(IS_TERMINATED)return ARC_USER_CANCEL;

		//圧縮
		tstring log;
		bool result=false;

		switch(m_arc_dll->compress(m_cur_file.arc_path.c_str(),&compress_file_list,&log)){
			case Archiver::ARC_SUCCESS:
				result=true;
				break;
			case Archiver::ARC_CANNOT_OPEN_LISTFILE:
				err_msg=_T("リストファイルを開くことが出来ませんでした。\n");
				break;
			case Archiver::ARC_NO_MATCHES_FOUND:
				err_msg=_T("フィルタに一致するファイルはありません。\n");
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

		if(!result){
			msg::lasterr();
		}

		if(!CFG.no_display.no_log){
			STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log.c_str());
		}

		if(split_file){
			//分割処理
			msg::info(_T("ファイルを分割しています...\n"));
			switch(splitfile::splitFile(m_cur_file.arc_path.c_str(),CFG.compress.split_value.c_str(),orig_dir.c_str())){
				case splitfile::split::SUCCESS:
					//一時ディレクトリ内のファイル/ディレクトリを削除
					fileoperation::deleteContents(m_split_temp_dir.c_str());
					return ARC_SUCCESS;
				case splitfile::split::CANNOT_OPEN:
					err_msg=_T("分割対象ファイル '%s' を開くことが出来ませんでした。\n"),m_cur_file.arc_path.c_str();
					break;
				case splitfile::split::INVALID_PARAM:
					err_msg=_T("分割サイズまたは分割数が正しくありません。\n");
					break;
				case splitfile::split::NOT_NECESSARY:
					err_msg=_T("分割する必要がありません。\n");
					break;
				case splitfile::split::TOO_MANY:
					err_msg=_T("分割数が多すぎます。\n");
					break;
				case splitfile::split::MALLOC_ERR:
					err_msg=_T("メモリの確保に失敗しました。\n");
					break;
				case splitfile::split::CANNOT_CREATE:
					err_msg=_T("分割ファイルの作成に失敗しました。\n");
					break;
			}
			//エラーであってもとりあえずコピー
			//ファイル名を作成
			orig_dir+=_T("\\")+path::getFileName(m_cur_file.arc_path);
			::CopyFile(m_cur_file.arc_path.c_str(),orig_dir.c_str(),false);
			//一時ディレクトリ内のファイル/ディレクトリを削除
			fileoperation::deleteContents(m_split_temp_dir.c_str());
		}
		return (result)?ARC_SUCCESS:ARC_FAILURE;
	}
	return ARC_FAILURE;
}
