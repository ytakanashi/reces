//Arc7zip32.cpp
//7-zip32.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r29 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"Arc7-zip32.h"
#include"Hook/HookArchiverDialog.h"
#include"ArcCfg.h"

using namespace sslib;



Arc7zip32::Arc7zip32():
	ArcDll(
#ifndef _WIN64
			_T("7-zip32"),
#else
			_T("7-zip64"),
#endif
			_T("SevenZip"),
			_T("zip.7z.jar"),
			_T("\\")),
			m_file_size(0),
			m_write_size(0){
		COMPRESSION_FORMAT format[]={
			{_T("zip"),_T(".zip"),_T("-tzip"),_T("-mx="),
				0,
				5,0,9},
			{_T("zippw"),_T(".zip"),_T("-tzip -p"),_T("-mx="),
				MHD_PASSWORD,
				5,0,9},

			{_T("7z"),_T(".7z"),_T("-t7z"),_T("-mx="),
				0,
				5,0,9},
			{_T("7zpw"),_T(".7z"),_T("-t7z -p"),_T("-mx="),
				MHD_PASSWORD,
				5,0,9},
			{_T("7zhe"),_T(".7z"),_T("-t7z -p -mhe=on"),_T("-mx="),
				MHD_HEADERENCRYPTION,
				5,0,9},
			{_T("7zsfx"),_T(".exe"),_T("-t7z -sfx"),_T("-mx="),
				MHD_SFX,
				5,0,9},
			{_T("7zpwsfx"),_T(".exe"),_T("-t7z -sfx7z.sfx -p"),_T("-mx="),
				MHD_PASSWORD|MHD_SFX,
				5,0,9},
			{_T("7zsfxpw"),_T(".exe"),_T("-t7z -sfx7z.sfx -p"),_T("-mx="),
				MHD_PASSWORD|MHD_SFX,
				5,0,9},
			{_T("7zhesfx"),_T(".exe"),_T("-t7z -sfx7z.sfx -p -mhe=on"),_T("-mx="),
				MHD_HEADERENCRYPTION|MHD_SFX,
				5,0,9},
			{_T("7zsfxhe"),_T(".exe"),_T("-t7z -sfx7z.sfx -p -mhe=on"),_T("-mx="),
				MHD_HEADERENCRYPTION|MHD_SFX,
				5,0,9},
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_formats.assign(format,format+ARRAY_SIZEOF(format));
}

//対応している書庫であるか
bool Arc7zip32::isSupportedArchive(const TCHAR* arc_path_orig,int mode){
	bool result=false;

	tstring arc_path(arc_path_orig);
	replaceDelimiter(arc_path);

	setDefaultPassword(NULL);

#if 0
	//CHECKARCHIVE_RAPIDにするとFindFirst()で処理をすることになるので
	//CHECKARCHIVE_BASICで。
	if(mode==CHECKARCHIVE_RAPID){
		bool rapid_result=checkArchive(arc_path.c_str(),CHECKARCHIVE_RAPID);

		return rapid_result;
	}
#endif

	bool use_password=!CFG.general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=CFG.general.password_list.begin();
	std::list<tstring>::iterator password_list_end=CFG.general.password_list.end();

	do{
		if(use_password&&ite_password_list!=password_list_end){
			CFG.general.password=*ite_password_list;
			++ite_password_list;
		}else{
			CFG.general.password.clear();
		}
		result=checkArchive(arc_path.c_str(),CHECKARCHIVE_BASIC);
	}while(!IS_TERMINATED&&
		   !ARCCFG->m_password_input_cancelled&&
		   //正しいパスワードが入力されるまで問い合わせる
		   !result&&
		   ARCCFG->m_hook_dialog_type==HOOK_7ZIP32_PASSWORD);

	if(result&&
	   !CFG.general.password.empty()){
		//ここで設定しないとOpenArchive()が失敗してしまう?
		setDefaultPassword(CFG.general.password.c_str());
	}

	return result;
}

//書庫をテスト
Arc7zip32::ARC_RESULT Arc7zip32::test(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	tstring cmd_line(format(_T("%s %s "),
							_T("t"),
							_T("-hide")));

	//文字コードの設定
	if(CFG.general.arc_codepage){
		cmd_line.append(format(_T("-mcp=%d "),
							   CFG.general.arc_codepage));
		setUnicodeMode(CFG.general.arc_codepage);
	}

	cmd_line.append(format(_T("%s %s"),
							_T("--"),
							path::quote(arc_path_str).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	bool use_password=!CFG.general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=CFG.general.password_list.begin();
	std::list<tstring>::iterator password_list_end=CFG.general.password_list.end();

	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	tstring log_msg;

	do{
		if(use_password&&ite_password_list!=password_list_end){
			CFG.general.password=*ite_password_list;
			++ite_password_list;
		}else{
			CFG.general.password.clear();
		}

		if(CFG.no_display.no_log){
			//実行
			tstring dummy(1,'\0');

			dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
		}else{
			dll_ret=execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
		}
	}while(!IS_TERMINATED&&
		   //正しいパスワードが入力されるまで問い合わせる
		   dll_ret==ERROR_PASSWORD_FILE);

	if(!CFG.no_display.no_log){
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

//圧縮形式を取得(その形式に対応している場合のみ)
tstring Arc7zip32::getCompressionFormat(const TCHAR* arc_path_orig){
	tstring arc_path(arc_path_orig);
	replaceDelimiter(arc_path);

	switch(getArchiveType(arc_path.c_str())){
		case ARCHIVETYPE_ZIP:
			return _T("zip");
			break;
		case ARCHIVETYPE_7Z:
			return _T("7z");
			break;
	}
	return _T("");
}

Arc7zip32::ARC_RESULT Arc7zip32::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	if(getFormat().mhd==NULL)return ARC_FAILURE;

	tstring arc_path_str(arc_path);

	m_file_size=m_write_size=0;

	//階層無視圧縮ではリスト出力が必要!
	if(CFG.general.ignore_directory_structures&&
	   (CFG.general.filefilter.empty()&&CFG.general.file_ex_filter.empty())){
		CFG.general.filefilter.pattern_list.push_back(tstring(_T("*")));
	}

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
	bool use_exclude_list=false;

	//リストファイルにパスを出力
	File list_file;
	tstring list_file_path;
	File exclude_list_file;
	tstring exclude_list_file_path;

	if(CFG.mode==MODE_RECOMPRESS){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("7z"),ARCCFG->m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARC_CANNOT_OPEN_LISTFILE;
		}

		list_file.writeEx(_T("*\r\n"));
	}else{
		for(std::list<tstring>::const_iterator ite=file_list->begin(),
			begin=file_list->begin(),
			end=file_list->end();
			ite!=end;
			++ite){
			///mcかつリストを分割
			if(ite==begin){
				//mcかつフィルタ有りなら処理対象外リストファイルを作成
				if(!CFG.general.ignore_directory_structures&&
				   use_filter){
					exclude_list_file_path=tempfile::create(_T("7z"),ARCCFG->m_list_temp_dir.c_str());
					if(!exclude_list_file.open(exclude_list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
						return ARC_CANNOT_OPEN_LISTFILE;
					}
				}

				//リストファイルを作成
				list_file_path=tempfile::create(_T("7z"),ARCCFG->m_list_temp_dir.c_str());
				if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
					return ARC_CANNOT_OPEN_LISTFILE;
				}
			}

			//リストを出力(フィルタが有効であれば処理対象外リストも出力)
			writeFormatedList(list_file,exclude_list_file,*ite);

			//区切り文字置換
			replaceDelimiter(list_file_path);
		}

		if(list_file.getSize()==0){
			return ARC_NO_MATCHES_FOUND;
		}

		if(!CFG.general.ignore_directory_structures&&
		   use_filter){
			use_exclude_list=exclude_list_file.getSize()!=0;
			exclude_list_file.close();
		}
	}

	exclude_list_file.close();
	list_file.close();

	if(IS_TERMINATED)return ARC_FAILURE;

	replaceDelimiter(exclude_list_file_path);
	replaceDelimiter(list_file_path);

	//圧縮率の処理
	tstring level_str;

	if(CFG.compress.compression_level==minimum_compressionlevel){
		//'/l'と指定された場合
		CFG.compress.compression_level=getFormat().minimum_level;
	}else if(CFG.compress.compression_level==maximum_compressionlevel){
		//'/lx'と指定された場合
		CFG.compress.compression_level=getFormat().maximum_level;
	}

	if(CFG.compress.compression_level!=default_compressionlevel){
		//圧縮率を範囲内に収める
		CFG.compress.compression_level=clamp(CFG.compress.compression_level,
											   getFormat().minimum_level,
											   getFormat().maximum_level);

		level_str=getFormat().level;
		level_str+=static_cast<TCHAR>(CFG.compress.compression_level+'0');
	}

	if(CFG.compress.force_create_new&&
	   path::fileExists(arc_path)){
		//出力先が存在すれば削除
		::DeleteFile(arc_path);
	}

	//区切り文字置換
	replaceDelimiter(arc_path_str);
	replaceDelimiter(&CFG.general.filefilter.pattern_list);
	replaceDelimiter(&CFG.general.file_ex_filter.pattern_list);
	replaceDelimiter(file_list);

	//勝手に拡張子が付加されないように'.'をファイル名末尾に追加。
	arc_path_str+=_T(".");

	if(CFG.mode==MODE_RECOMPRESS){
		//再圧縮する書庫のパスワードを新たに設定
		if(!CFG.recompress.new_password.empty()){
			CFG.general.password=CFG.recompress.new_password;
		}
	}

	msg::info(_T("'%s'に圧縮しています...\n\n"),arc_path);

	tstring cmd_line(format(_T("%s %s %s %s %s %s "),
							_T("a"),
							(CFG.compress.create_new)?_T("-y -up0q0"):_T("-y"),
							//-y     : 全ての質問に yes を仮定。
							//-u     :
							//  p    :ファイルは書庫内に存在するがワイルドカード名と一致しない。
							//  q    :ファイルは書庫内に存在するがディスク上には存在しない。
							getFormat().cmd,
							(CFG.compress.compression_level!=default_compressionlevel&&!level_str.empty())?level_str.c_str():_T(""),
							CFG.general.custom_param.c_str(),
							_T("-hide")));

	if(use_exclude_list){
		//ディレクトリを/mcで圧縮(with フィルタ)する場合
		//-xスイッチで処理対象外リストを指定
		cmd_line.append(format(_T("-x@%s "),
							   path::quote(exclude_list_file_path).c_str()));
	}

	cmd_line.append(format(_T("%s %s @%s"),
						   _T("--"),
						   path::quote(arc_path_str).c_str(),
						   path::quote(list_file_path).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

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

	unload();

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

Arc7zip32::ARC_RESULT Arc7zip32::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	tstring list_file_path;
	File list_file;

	if(CFG.general.ignore_directory_structures){
		//7-zip32系のライブラリは'e'解凍でディレクトリを作成してしまうので、
		//解凍対象ファイルをリストファイルに書き出して、作成を抑制する
		if(CFG.general.filefilter.empty()){
			CFG.general.filefilter.pattern_list.push_back(_T("*"));
		}
	}

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	//文字コードの設定
	if(CFG.general.arc_codepage)setUnicodeMode(CFG.general.arc_codepage);

	if(use_filter){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("7z"),ARCCFG->m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARC_CANNOT_OPEN_LISTFILE;
		}

		//リストファイルに解凍対象外ファイルを出力
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

	//文字コードの設定
	if(CFG.general.arc_codepage){
		cmd_line.append(format(_T("-mcp=%d "),
							   CFG.general.arc_codepage));
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

	bool use_password=!CFG.general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=CFG.general.password_list.begin();
	std::list<tstring>::iterator password_list_end=CFG.general.password_list.end();

	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	do{
		if(use_password){
			CFG.general.password=*ite_password_list;
		}

		//実行
		if(CFG.no_display.no_log||log_msg==NULL){
			tstring dummy(1,'\0');

			dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
		}else{
			dll_ret=execute(NULL,cmd_line.c_str(),log_msg,log_buffer_size);
		}
	}while(use_password&&
		   dll_ret!=0&&
		   (++ite_password_list)!=password_list_end);

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

Arc7zip32::ARC_RESULT Arc7zip32::del(const TCHAR* arc_path_orig,tstring* log_msg){
	tstring arc_path(arc_path_orig);

	//文字コードの設定
	if(CFG.general.arc_codepage)setUnicodeMode(CFG.general.arc_codepage);

	tstring list_file_path;
	File list_file;
	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	if(!use_filter
	   /*||
	   !CFG.recompress.run_command.disable()||
	   CFG.general.ignore_directory_structures||
	   CFG.compress.compression_level!=default_compressionlevel
	   */){
		return ARC_NO_FILTER;
	}

	//リストファイルを作成
	list_file_path=tempfile::create(_T("7z"),ARCCFG->m_list_temp_dir.c_str());
	if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
		return ARC_CANNOT_OPEN_LISTFILE;
	}

	//リストファイルに解凍対象ファイルのみ出力
	outputFileListEx(arc_path.c_str(),
					 CFG.general.filefilter,
					 CFG.general.file_ex_filter,
					 //フィルタ適用を逆にする
					 REVERSE_FILTER,
					 &list_file);
	list_file.close();

	//区切り文字置換
	replaceDelimiter(arc_path);
	replaceDelimiter(list_file_path);

	tstring cmd_line(format(_T("%s %s %s -t%s "),
							_T("d"),
							_T("-y"),
							//-y     : 全ての質問に yes を仮定
							_T("-hide"),

							//書庫形式指定
							//注意:7-zip32.dll 9.38.00.01以降だとgetArchiveType()に.を付加したファイル名を渡すとエラーとなる
							getCompressionFormat(arc_path.c_str()).c_str()));

	//文字コードの設定
	if(CFG.general.arc_codepage){
		cmd_line.append(format(_T("-mcp=%d "),
							   CFG.general.arc_codepage));
	}

	cmd_line.append(format(_T("%s %s @%s"),
							_T("--"),

							//勝手に拡張子が付加されないように'.'をファイル名末尾に追加
							path::quote(arc_path+_T(".")).c_str(),
							path::quote(list_file_path).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());
	msg::info(_T("'%s'を処理しています...\n\n"),arc_path_orig);

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

	unload();

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

Arc7zip32::ARC_RESULT Arc7zip32::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	//区切り文字置換
	replaceDelimiter(arc_path_str);

	//文字コードの設定
	if(CFG.general.arc_codepage)setUnicodeMode(CFG.general.arc_codepage);

	if(CFG.output_file_list.api_mode){
//		setDefaultPassword(CFG.general.password.c_str());
		outputFileListEx(arc_path_str.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter);
//		setDefaultPassword(NULL);
		return ARC_SUCCESS;
	}

	tstring list_file_path;
	File list_file;

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	if(use_filter){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("7z"),ARCCFG->m_list_temp_dir.c_str());
		if(list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			//リストファイルに列挙対象外ファイルを出力
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

	//文字コードの設定
	if(CFG.general.arc_codepage){
		cmd_line.append(format(_T("-mcp=%d "),
							   CFG.general.arc_codepage));
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
void Arc7zip32::applyFilters(std::vector<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse){
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
DWORD Arc7zip32::writeFormatedPath(const File& list_file,const TCHAR* base_dir,const TCHAR* file_path){
	return list_file.writeEx(_T("%s\r\n"),
							 path::quote(tstring(base_dir)+m_delimiter+file_path).c_str());
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD Arc7zip32::writeFormatedPath(const File& list_file,const TCHAR* file_path){
	return list_file.writeEx(_T("%s\r\n"),
							 path::quote(file_path).c_str());
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool Arc7zip32::writeFormatedList(const File& list_file,const tstring& full_path){
	return false;
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool Arc7zip32::writeFormatedList(const File& list_file,File& exclude_list_file,const tstring& full_path){
	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
	bool use_exclude_list=false;

	tstring base_dir(path::getParentDirectory(full_path));
	tstring file_name(path::getFileName(full_path));

	if(use_filter){
		fileinfo::FILEINFO fileinfo=fileinfo::FILEINFO();

		if(getFileInfo(&fileinfo,full_path.c_str())){
			//i:ad,/x:adチェックを先に行う。
			if(!filter::matchAttributes(fileinfo,CFG.general.filefilter,CFG.general.file_ex_filter,false)){
				return false;
			}
		}
	}

	fileinfo::FILEFILTER filefilter=CFG.general.filefilter;
	fileinfo::FILEFILTER file_ex_filter=CFG.general.file_ex_filter;

	//FILE_ATTRIBUTE_DIRECTORYを取り除く
	filefilter.attr&=~FILE_ATTRIBUTE_DIRECTORY;
	file_ex_filter.attr&=~FILE_ATTRIBUTE_DIRECTORY;

	if(path::isDirectory(full_path.c_str())){
		//full_pathはディレクトリ

		//フィルタ使用時のみ使用
		std::list<size_info> relative_path_list;

		if(use_filter){
			//フィルタ使用時
			std::vector<fileinfo::FILEINFO> filtered_list;

			FileTree file_tree(filefilter,file_ex_filter);

			file_tree.createFileTree(full_path.c_str(),_T("*"),true);

			if(!CFG.general.ignore_directory_structures){
				//通常圧縮->処理対象外ファイルリスト
				file_tree.makeExcludeTree(FileTree::TO_NONE,base_dir.c_str());
				use_exclude_list=true;
			}else{
				//階層無視圧縮->処理対象ファイルリスト出力(全てのディレクトリを含まない)
				file_tree.makeIncludeTree(FileTree::TO_EXCLUDE_DIR,base_dir.c_str());
			}

			//リストに変換
			file_tree.tree2list(filtered_list);

			if(filtered_list.empty()){
				//フィルタを使用しない処理へ
				use_filter=false;
			}else{
				for(std::vector<fileinfo::FILEINFO>::iterator ite=filtered_list.begin(),
					end=filtered_list.end();
					ite!=end;
					++ite){
					if(use_exclude_list){
						//処理対象外リストは[親ディレクトリ基準の]相対パス
						///eb時は[対象ディレクトリ基準の]相対パス
						relative_path_list.push_back(size_info(
							path::makeRelativePath((CFG.compress.exclude_base_dir==0)?
												   base_dir.c_str():full_path.c_str(),
												   FILE_ATTRIBUTE_DIRECTORY,
												   ite->name.c_str(),
												   ::GetFileAttributes(ite->name.c_str())),
							ite->size));
					}else{
						//処理対象リストはフルパス
						relative_path_list.push_back(size_info(ite->name,ite->size));
					}
				}
			}
		}

		if(use_filter&&relative_path_list.empty()){
			return false;
		}

		if(!CFG.general.ignore_directory_structures){
			//処理対象リストに書き出し
			if(CFG.compress.exclude_base_dir==0){
				writeFormatedPath(list_file,full_path.c_str());
			}else{
				//'C:\~\DIR\*'
				file_name=path::addTailSlash(full_path);
				file_name+=_T("*");
				writeFormatedPath(list_file,file_name.c_str());
			}
		}

		if(!use_filter){
			//合計サイズ
			m_file_size+=fileoperation::getDirectorySize(full_path.c_str());
		}else if(!relative_path_list.empty()){
			long long exclude_file_size=0;

			for(std::list<size_info>::iterator ite=relative_path_list.begin(),
				end=relative_path_list.end();
				ite!=end;
				++ite){
				if(!CFG.general.ignore_directory_structures){
					//'/mc'
					//write:
					//[ディレクトリ内ファイルの相対パス]
					//処理対象外リストに書き出し
					writeFormatedPath(exclude_list_file,ite->first.c_str());

					//合計サイズ[処理対象外]
					exclude_file_size+=ite->second;
				}else{
					//'mC'
					//write:
					//[ファイルのフルパス]
					writeFormatedPath(list_file,ite->first.c_str());

					//合計サイズ
					m_file_size+=ite->second;
				}
			}

			//合計サイズ
			if(!CFG.general.ignore_directory_structures){
				m_file_size+=fileoperation::getDirectorySize(full_path.c_str())-exclude_file_size;
			}
		}
	}else{

		//full_pathはファイル
		if(fileinfo::matchFilters(full_path.c_str(),filefilter,file_ex_filter,base_dir.c_str())){
			if(!CFG.general.ignore_directory_structures){
				//'/mc'
				//write:
				//[相対パス]
				writeFormatedPath(list_file,full_path.c_str());
			}else{
				//'mC'
				//write:
				//[ファイルのフルパス]
				writeFormatedPath(list_file,base_dir.c_str(),file_name.c_str());
			}
			//合計サイズ
			m_file_size+=fileoperation::getFileSize(full_path.c_str());
		}
	}
	return true;
}
