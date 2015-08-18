﻿//ArcUniso32.cpp
//Uniso32.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r29 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcUniso32.h"
#include"ArcCfg.h"

using namespace sslib;



ArcUniso32::ArcUniso32():
	ArcDll(_T("Uniso32"),
			_T("UnIso"),
			_T("iso"),
			_T("\\")){
		COMPRESSION_FORMAT format[]={
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_formats.assign(format,format+ARRAY_SIZEOF(format));
}

//書庫をテスト
ArcUniso32::ARC_RESULT ArcUniso32::test(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	tstring cmd_line(format(_T("%s %s %s %s"),
							_T("t"),
							_T("-hide"),
							_T("--"),
							path::quote(arc_path_str).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	int dll_ret=-1;

	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	if(CFG.no_display.no_log){
		//実行
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
	}else{
		tstring log_msg;

		dll_ret=execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcUniso32::ARC_RESULT ArcUniso32::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	return ARC_NOT_IMPLEMENTED;
}

ArcUniso32::ARC_RESULT ArcUniso32::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	tstring list_file_path;
	File list_file;

	if(CFG.general.ignore_directory_structures){
		//7-zip32系のライブラリは'e'解凍でディレクトリを作成してしまうので、
		//解凍対象ファイルをリストファイルに書き出して、作成を抑制する
		if(CFG.general.filefilter.empty()){
			CFG.general.filefilter.pattern_list.push_back(tstring(_T("*")));
		}
	}

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	if(use_filter){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("iso"),ARCCFG->m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARC_CANNOT_OPEN_LISTFILE;
		}

		//リストファイルに解凍対象ファイルのみ出力
		outputFileListEx(arc_path_str.c_str(),
						 CFG.general.filefilter,
						 CFG.general.file_ex_filter,
						 //フィルタ適用を逆にする
						 (!CFG.general.ignore_directory_structures)?REVERSE_FILTER:0,
						 &list_file);
		list_file.close();
	}

	if(IS_TERMINATED)return ARC_FAILURE;

	tstring output_dir_bak;
	fileoperation::scheduleDelete schedule_delete;

	if(CFG.compress.exclude_base_dir!=0){
		output_dir_bak=output_dir_str;
		output_dir_str=path::addTailSlash(tempfile::createDir(_T("rcs"),output_dir_str.c_str()));

		//一時ディレクトリ削除予約
		schedule_delete.set(output_dir_str.c_str());
		//削除漏れ対策
		ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(output_dir_str.c_str()));
	}

	//区切り文字置換
	replaceDelimiter(arc_path_str);
	replaceDelimiter(output_dir_str);

	tstring cmd_line(format(_T("%s %s %s %s "),
							(!CFG.general.ignore_directory_structures)?_T("x"):_T("e"),
							_T("-y -aoa"),
							//-y     : 全ての質問に yes を仮定。
							//-aoa   : 全てのファイルを確認しないで上書きします。
							CFG.general.custom_param.c_str(),
							_T("-hide")));

	if(use_filter&&
	   !CFG.general.ignore_directory_structures){
		//通常解凍(with フィルタ)する場合
		//-xスイッチで処理対象外リストを指定
		cmd_line.append(format(_T("-x@%s "),
							   path::quote(list_file_path).c_str()));
	}

#if 1
	//-oで指定するディレクトリにスペースが2つ以上含まれると1つに削られてしまうので予めSetCurrentDirectory()で移動しておく
	fileoperation::temporaryCurrentDirectory temp_dir(output_dir_str.c_str());
	cmd_line.append(format(_T("%s %s"),
						   _T("--"),
						   path::quote(arc_path_str).c_str()));
#else
	cmd_line.append(format(_T("-o%s %s %s"),
						   path::quote(output_dir_str).c_str(),
						   _T("--"),
						   path::quote(arc_path_str).c_str()));
#endif

	if(use_filter&&
	   CFG.general.ignore_directory_structures){
		cmd_line.append(format(_T(" @%s"),
							   path::quote(list_file_path).c_str()));
	}

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	msg::info(_T("'%s'を解凍しています...\n\n"),arc_path);

	//実行
	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	if(CFG.no_display.no_log||log_msg==NULL){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.c_str(),log_msg,log_buffer_size);
	}

	msg::info(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	//パス区切り文字を'\\'に
	if(*m_delimiter=='/')str::replaceCharacter(output_dir_str,'/','\\');

	if(!CFG.general.ignore_directory_structures&&
	   CFG.extract.directory_timestamp){
		//ディレクトリの更新日時を復元
		m_util->recoverDirectoryTimestamp(arc_path,output_dir_str.c_str(),true);
	}

	unload();

	if(CFG.compress.exclude_base_dir!=0){
		//共通パスを取り除く
		m_util->excludeCommonPath(output_dir_bak.c_str(),output_dir_str.c_str(),CFG.compress.exclude_base_dir);
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcUniso32::ARC_RESULT ArcUniso32::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	if(CFG.output_file_list.api_mode){
		outputFileListEx(arc_path_str.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter);
		return ARC_SUCCESS;
	}

	tstring list_file_path;
	File list_file;

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	if(use_filter){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("iso"),ARCCFG->m_list_temp_dir.c_str());
		if(list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			//リストファイルに解凍対象ファイルのみ出力
			outputFileListEx(arc_path_str.c_str(),
							 CFG.general.filefilter,
							 CFG.general.file_ex_filter,
							 //フィルタ適用を逆にする
							 REVERSE_FILTER,
							 &list_file);
			list_file.close();
		}else{
			use_filter=false;
		}
	}

	tstring cmd_line(format(_T("%s %s "),
							_T("l"),
							_T("-hide")));
	if(use_filter){
		//-xスイッチで処理対象外リストを指定
		cmd_line.append(format(_T("-x@%s "),
							   path::quote(list_file_path).c_str()));
	}

	cmd_line.append(format(_T("%s %s"),
						   _T("--"),
						   path::quote(arc_path_str).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	tstring log_msg;

	execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
	STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());

	return ARC_SUCCESS;
}

//リストにフィルタを適用
void ArcUniso32::applyFilters(std::vector<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse){
	if(filefilter.empty()&&file_ex_filter.empty())return;

	ArcDll::applyFilters(fileinfo_list,filefilter,file_ex_filter,reverse);

	if(CFG.general.ignore_directory_structures){
		for(std::vector<fileinfo::FILEINFO>::iterator ite=fileinfo_list->begin();
			ite!=fileinfo_list->end();){
			//attr==FILE_ATTRIBUTE_DIRECTORYだけで十分
			//ただし念のために
			if(ite->name.rfind(_T("\\"))==ite->name.size()-1||
			   ite->attr==FILE_ATTRIBUTE_DIRECTORY){
				ite=fileinfo_list->erase(ite);
				continue;
			}
			++ite;
		}
	}
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcUniso32::writeFormatedPath(const File& list_file,const TCHAR* base_dir,const TCHAR* file_path){
	return -1;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcUniso32::writeFormatedPath(const File& list_file,const TCHAR* file_path){
	return -1;
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcUniso32::writeFormatedList(const File& list_file,const tstring& full_path){
	return false;
}
