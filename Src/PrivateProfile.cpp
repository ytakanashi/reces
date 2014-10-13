//PrivateProfile.cpp
//設定

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r23 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"PrivateProfile.h"

using namespace sslib;


CONFIG Config::m_cfg;
const CONFIG Config::m_default_cfg;

//PrivateProfileファイルへ書き込む
bool Config::save(){
	//動作モード
	write(_T("Mode"),_T("Mode"),m_cfg.mode,m_cfg.mode!=m_default_cfg.mode);

	//一般
	//バックグラウンドで動作
	write(_T("General"),_T("Background"),m_cfg.general.background_mode,m_cfg.general.background_mode!=m_default_cfg.general.background_mode);
	//処理終了後ウインドウを閉じる
	write(_T("General"),_T("Quit"),m_cfg.general.quit,m_cfg.general.quit!=m_default_cfg.general.quit);
	//ディレクトリ階層を無視して圧縮/解凍
	write(_T("General"),_T("IgnoreDirectoryStructures"),m_cfg.general.ignore_directory_structures,m_cfg.general.ignore_directory_structures!=m_default_cfg.general.ignore_directory_structures);
	//指定したライブラリ名
	write(_T("General"),_T("LibraryName"),m_cfg.general.selected_library_name.c_str(),!m_cfg.general.selected_library_name.empty());
	//指定したライブラリのプレフィックス(msのみ)
	write(_T("General"),_T("LibraryPrefix"),m_cfg.general.selected_library_prefix.c_str(),!m_cfg.general.selected_library_prefix.empty());
	//出力先ディレクトリ
	write(_T("General"),_T("OutputDir"),m_cfg.general.output_dir.c_str(),!m_cfg.general.output_dir.empty());
	//カレントディレクトリ基準で'/od','/of'の相対パスを処理
	write(_T("General"),_T("DefaultBaseDir"),m_cfg.general.default_base_dir,m_cfg.general.default_base_dir!=m_default_cfg.general.default_base_dir);
	//出力ファイルが重複する場合リネーム
	write(_T("General"),_T("AutoRename"),m_cfg.general.auto_rename,m_cfg.general.auto_rename!=m_default_cfg.general.auto_rename);
	//ソースを削除
	write(_T("General"),_T("RemoveSource"),m_cfg.general.remove_source,m_cfg.general.remove_source!=m_default_cfg.general.remove_source);
	//ユーザ独自のパラメータ
	write(_T("General"),_T("CustomParam"),m_cfg.general.custom_param.c_str(),!m_cfg.general.custom_param.empty());
	//spiがあるディレクトリ
	write(_T("General"),_T("SpiDir"),m_cfg.general.spi_dir.c_str(),!m_cfg.general.spi_dir.empty());
	//b2eがあるディレクトリ
	write(_T("General"),_T("B2eDir"),m_cfg.general.b2e_dir.c_str(),!m_cfg.general.b2e_dir.empty());
	//wcxがあるディレクトリ
	write(_T("General"),_T("WcxDir"),m_cfg.general.wcx_dir.c_str(),!m_cfg.general.wcx_dir.empty());
	//Unicodeエスケープシーケンスをデコードする
	write(_T("General"),_T("DecodeUnicodeEscape"),m_cfg.general.decode_uesc,m_cfg.general.decode_uesc!=m_default_cfg.general.decode_uesc);


	//非表示
	//進捗状況を表示しない
	write(_T("NoDisplay"),_T("Information"),m_cfg.no_display.no_information,m_cfg.no_display.no_information!=m_default_cfg.no_display.no_information);
	//ログを表示しない
	write(_T("NoDisplay"),_T("Log"),m_cfg.no_display.no_log,m_cfg.no_display.no_log!=m_default_cfg.no_display.no_log);
	//パスワードを表示しない
	write(_T("NoDisplay"),_T("Password"),m_cfg.no_display.no_password,m_cfg.no_display.no_password!=m_default_cfg.no_display.no_password);
	//エラーメッセージを表示しない
	write(_T("NoDisplay"),_T("ErrorMessage"),m_cfg.no_display.no_errmsg,m_cfg.no_display.no_errmsg!=m_default_cfg.no_display.no_errmsg);


	//再圧縮
	//コマンド実行
	//対話形式で指定
	write(_T("RunCommand"),_T("Interactive"),m_cfg.recompress.run_command.interactive,m_cfg.recompress.run_command.interactive!=m_default_cfg.recompress.run_command.interactive);
	//コマンド
	write(_T("RunCommand"),_T("Command"),m_cfg.recompress.run_command.command.c_str(),!m_cfg.recompress.run_command.command.empty());


	//圧縮
	//圧縮形式
	write(_T("Compress"),_T("Type"),m_cfg.compress.compression_type.c_str(),!m_cfg.compress.compression_type.empty()&&!str::isEqualStringIgnoreCase(m_cfg.compress.compression_type,tstring(_T("zip"))));
	//個別圧縮
	write(_T("Compress"),_T("EachFile"),m_cfg.compress.each_file,m_cfg.compress.each_file!=m_default_cfg.compress.each_file);
	//書庫新規作成
	write(_T("Compress"),_T("CreateNew"),m_cfg.compress.create_new,m_cfg.compress.create_new!=m_default_cfg.compress.create_new);
	//基底ディレクトリを含まない
	if(m_cfg.compress.exclude_base_dir<-1)m_cfg.compress.exclude_base_dir=-1;
	write(_T("Compress"),_T("ExcludeBaseDir"),m_cfg.compress.exclude_base_dir,m_cfg.compress.exclude_base_dir!=m_default_cfg.compress.exclude_base_dir);
	//圧縮率
	write(_T("Compress"),_T("Level"),m_cfg.compress.compression_level,m_cfg.compress.compression_level!=m_default_cfg.compress.compression_level);
	//書庫分割数値
	write(_T("Compress"),_T("Split"),m_cfg.compress.split_value.c_str(),!m_cfg.compress.split_value.empty());
	//出力ファイル名
	write(_T("Compress"),_T("OutputFile"),m_cfg.compress.output_file.c_str(),!m_cfg.compress.output_file.empty());
	//'/of'以下の引数をそのまま使用
	write(_T("Compress"),_T("RawFileName"),m_cfg.compress.raw_file_name,m_cfg.compress.raw_file_name!=m_default_cfg.compress.raw_file_name);
	//更新日時を元書庫と同じにする
	write(_T("Compress"),_T("CopyTimestamp"),m_cfg.compress.copy_timestamp,m_cfg.compress.copy_timestamp!=m_default_cfg.compress.copy_timestamp);


	//解凍
	//ディレクトリを作成する
	write(_T("Extract"),_T("CreateDir"),m_cfg.extract.create_dir,m_cfg.extract.create_dir!=m_default_cfg.extract.create_dir);
	//二重ディレクトリを防ぐ
	write(_T("Extract"),_T("DoubleDir"),m_cfg.extract.create_dir_optimization.remove_redundant_dir.double_dir,m_cfg.extract.create_dir_optimization.remove_redundant_dir.double_dir!=m_default_cfg.extract.create_dir_optimization.remove_redundant_dir.double_dir);
	//ファイル単体の場合作成しない
	write(_T("Extract"),_T("OnlyFile"),m_cfg.extract.create_dir_optimization.remove_redundant_dir.only_file,m_cfg.extract.create_dir_optimization.remove_redundant_dir.only_file!=m_default_cfg.extract.create_dir_optimization.remove_redundant_dir.only_file);
	//ディレクトリ名末尾の数字を削除
	write(_T("Extract"),_T("OmitLastNumber"),m_cfg.extract.create_dir_optimization.omit_number_and_symbol.number,m_cfg.extract.create_dir_optimization.omit_number_and_symbol.number!=m_default_cfg.extract.create_dir_optimization.omit_number_and_symbol.number);
	//ディレクトリ名末尾の記号を削除
	write(_T("Extract"),_T("OmitLastSymbol"),m_cfg.extract.create_dir_optimization.omit_number_and_symbol.symbol,m_cfg.extract.create_dir_optimization.omit_number_and_symbol.symbol!=m_default_cfg.extract.create_dir_optimization.omit_number_and_symbol.symbol);
	//作成するディレクトリの更新日時を書庫と同じにする
	write(_T("Extract"),_T("CopyTimestamp"),m_cfg.extract.create_dir_optimization.copy_timestamp,m_cfg.extract.create_dir_optimization.copy_timestamp!=m_default_cfg.extract.create_dir_optimization.copy_timestamp);
	//パスワードリストファイル
	write(_T("Extract"),_T("PasswordList"),m_cfg.extract.password_list_path.c_str(),!m_cfg.extract.password_list_path.empty());
	//ディレクトリのタイムスタンプを復元する
	write(_T("Extract"),_T("DirTimestamp"),m_cfg.extract.directory_timestamp,m_cfg.extract.directory_timestamp!=m_default_cfg.extract.directory_timestamp);


	//一覧出力
	//FindFirst()/FindNext()/GetFileName()で出力する
	write(_T("OutputFileList"),_T("APIMode"),m_cfg.output_file_list.api_mode,m_cfg.output_file_list.api_mode!=m_default_cfg.output_file_list.api_mode);


	tstring attr_str;
	tstring pattern;

	//処理対象フィルタ
	if(!m_cfg.general.filefilter.empty()||
	   !isEmptySection(_T("FileFilter"))){
		VariableArgument min_size(_T("%I64d"),m_cfg.general.filefilter.min_size);
		VariableArgument max_size(_T("%I64d"),m_cfg.general.filefilter.max_size);

		write(_T("FileFilter"),_T("MinSize"),min_size.get(),m_cfg.general.filefilter.min_size!=m_default_cfg.general.filefilter.min_size);
		write(_T("FileFilter"),_T("MaxSize"),max_size.get(),m_cfg.general.filefilter.max_size!=m_default_cfg.general.filefilter.max_size);


		if(m_cfg.general.filefilter.attr!=m_default_cfg.general.filefilter.attr||
		   keyExists(_T("FileFilter"),_T("Attribute"))){
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_DIRECTORY)attr_str+='d';
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_HIDDEN)attr_str+='h';
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_READONLY)attr_str+='r';
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_SYSTEM)attr_str+='s';

			write(_T("FileFilter"),_T("Attribute"),attr_str.c_str(),m_cfg.general.filefilter.attr!=m_default_cfg.general.filefilter.attr);
		}

		write(_T("FileFilter"),_T("OldestDate"),str::longlong2datetime(m_cfg.general.filefilter.oldest_date).c_str(),m_cfg.general.filefilter.oldest_date!=m_default_cfg.general.filefilter.oldest_date);
		write(_T("FileFilter"),_T("NewestDate"),str::longlong2datetime(m_cfg.general.filefilter.newest_date).c_str(),m_cfg.general.filefilter.newest_date!=m_default_cfg.general.filefilter.newest_date);

		if(!m_cfg.general.filefilter.pattern_list.empty()||
		   keyExists(_T("FileFilter"),_T("Pattern"))){
			for(std::list<tstring>::iterator ite=m_cfg.general.filefilter.pattern_list.begin(),
				end=m_cfg.general.filefilter.pattern_list.end();
				ite!=end;
				++ite){
				if(ite!=m_cfg.general.filefilter.pattern_list.begin())pattern+=_T(";");
				pattern+=*ite;
			}
			write(_T("FileFilter"),_T("Pattern"),pattern.c_str(),!pattern.empty());
		}

		write(_T("FileFilter"),_T("Recursive"),m_cfg.general.filefilter.recursive,m_cfg.general.filefilter.recursive!=m_default_cfg.general.filefilter.recursive);
	}


	//処理対象外フィルタ
	if(!m_cfg.general.file_ex_filter.empty()||
	   !isEmptySection(_T("FileExFilter"))){
		VariableArgument min_size(_T("%I64d"),m_cfg.general.file_ex_filter.min_size);
		VariableArgument max_size(_T("%I64d"),m_cfg.general.file_ex_filter.max_size);

		write(_T("FileExFilter"),_T("MinSize"),min_size.get(),m_cfg.general.file_ex_filter.min_size!=m_default_cfg.general.file_ex_filter.min_size);
		write(_T("FileExFilter"),_T("MaxSize"),max_size.get(),m_cfg.general.file_ex_filter.max_size!=m_default_cfg.general.file_ex_filter.max_size);


		if(m_cfg.general.file_ex_filter.attr!=m_default_cfg.general.file_ex_filter.attr||
		   keyExists(_T("FileExFilter"),_T("Attribute"))){
			attr_str.clear();

			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY)attr_str+='d';
			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_HIDDEN)attr_str+='h';
			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_READONLY)attr_str+='r';
			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_SYSTEM)attr_str+='s';

			write(_T("FileExFilter"),_T("Attribute"),attr_str.c_str(),m_cfg.general.file_ex_filter.attr!=m_default_cfg.general.file_ex_filter.attr);
		}

		write(_T("FileExFilter"),_T("IncludeEmptyDir"),m_cfg.general.file_ex_filter.include_empty_dir,m_cfg.general.file_ex_filter.include_empty_dir!=m_default_cfg.general.file_ex_filter.include_empty_dir);

		write(_T("FileExFilter"),_T("OldestDate"),str::longlong2datetime(m_cfg.general.file_ex_filter.oldest_date).c_str(),m_cfg.general.file_ex_filter.oldest_date!=m_default_cfg.general.file_ex_filter.oldest_date);
		write(_T("FileExFilter"),_T("NewestDate"),str::longlong2datetime(m_cfg.general.file_ex_filter.newest_date).c_str(),m_cfg.general.file_ex_filter.newest_date!=m_default_cfg.general.file_ex_filter.newest_date);


		if(m_cfg.general.file_ex_filter.pattern_list!=m_default_cfg.general.file_ex_filter.pattern_list||
		   keyExists(_T("FileExFilter"),_T("Pattern"))){
			pattern.clear();

			for(std::list<tstring>::iterator ite=m_cfg.general.file_ex_filter.pattern_list.begin(),
				end=m_cfg.general.file_ex_filter.pattern_list.end();
				ite!=end;
				++ite){
				if(ite!=m_cfg.general.file_ex_filter.pattern_list.begin())pattern+=_T(";");
				pattern+=*ite;
			}
			write(_T("FileExFilter"),_T("Pattern"),pattern.c_str(),!pattern.empty());
		}

		write(_T("FileExFilter"),_T("Recursive"),m_cfg.general.file_ex_filter.recursive,m_cfg.general.file_ex_filter.recursive!=m_default_cfg.general.file_ex_filter.recursive);
	}


	return true;
}

//PrivateProfileファイルから読み込む
bool Config::load(){
	if(!path::fileExists(m_cfg_path.c_str()))return false;

	//動作モード
	getDataEx(_T("Mode"),_T("Mode"),&m_cfg.mode);


	//一般
	//バックグラウンドで動作
	getDataEx(_T("General"),_T("Background"),&m_cfg.general.background_mode);
	//処理終了後ウインドウを閉じる
	getDataEx(_T("General"),_T("Quit"),&m_cfg.general.quit,true);
	//ディレクトリ階層を無視して圧縮/解凍
	getDataEx(_T("General"),_T("IgnoreDirectoryStructures"),&m_cfg.general.ignore_directory_structures);
	//指定したライブラリ名
	getStringDataEx(_T("General"),_T("LibraryName"),&m_cfg.general.selected_library_name);
	//指定したライブラリのプレフィックス(msのみ)
	getStringDataEx(_T("General"),_T("LibraryPrefix"),&m_cfg.general.selected_library_prefix);
	//出力先ディレクトリ
	getStringDataEx(_T("General"),_T("OutputDir"),&m_cfg.general.output_dir);
	//カレントディレクトリ基準で'/od','/of'の相対パスを処理
	getDataEx(_T("General"),_T("DefaultBaseDir"),&m_cfg.general.default_base_dir);
	//出力ファイルが重複する場合リネーム
	getDataEx(_T("General"),_T("AutoRename"),&m_cfg.general.auto_rename);
	//ソースを削除
	getDataEx(_T("General"),_T("RemoveSource"),&m_cfg.general.remove_source);
	//ユーザ独自のパラメータ
	getStringDataEx(_T("General"),_T("CustomParam"),&m_cfg.general.custom_param);
	//spiのあるディレクトリ
	getStringDataEx(_T("General"),_T("SpiDir"),&m_cfg.general.spi_dir);
	//b2eのあるディレクトリ
	getStringDataEx(_T("General"),_T("B2eDir"),&m_cfg.general.b2e_dir);
	//wcxのあるディレクトリ
	getStringDataEx(_T("General"),_T("WcxDir"),&m_cfg.general.wcx_dir);
	//Unicodeエスケープシーケンスをデコードする
	getDataEx(_T("General"),_T("DecodeUnicodeEscape"),&m_cfg.general.decode_uesc);


	//非表示
	//情報を表示しない
	getDataEx(_T("NoDisplay"),_T("Information"),&m_cfg.no_display.no_information);
	//ログを表示しない
	getDataEx(_T("NoDisplay"),_T("Log"),&m_cfg.no_display.no_log);
	//パスワードを表示しない
	getDataEx(_T("NoDisplay"),_T("Password"),&m_cfg.no_display.no_password);
	//エラーメッセージを表示しない
	getDataEx(_T("NoDisplay"),_T("ErrorMessage"),&m_cfg.no_display.no_errmsg);

	//再圧縮
	//コマンド実行
	//対話形式で指定
	getDataEx(_T("RunCommand"),_T("Interactive"),&m_cfg.recompress.run_command.interactive);
	//コマンド
	getStringDataEx(_T("RunCommand"),_T("Command"),&m_cfg.recompress.run_command.command);


	//圧縮
	//圧縮形式
	getStringDataEx(_T("Compress"),_T("Type"),&m_cfg.compress.compression_type);
	//個別圧縮
	getDataEx(_T("Compress"),_T("EachFile"),&m_cfg.compress.each_file);
	//書庫個別圧縮
	getDataEx(_T("Compress"),_T("CreateNew"),&m_cfg.compress.create_new);
	//基底ディレクトリを含まない
	getDataEx(_T("Compress"),_T("ExcludeBaseDir"),&m_cfg.compress.exclude_base_dir);
	if(m_cfg.compress.exclude_base_dir<-1)m_cfg.compress.exclude_base_dir=-1;
	//圧縮率
	getDataEx(_T("Compress"),_T("Level"),&m_cfg.compress.compression_level);
	//書庫分割数値
	getStringDataEx(_T("Compress"),_T("Split"),&m_cfg.compress.split_value);
	//出力ファイル名
	getStringDataEx(_T("Compress"),_T("OutputFile"),&m_cfg.compress.output_file);
	//'/of'以下の引数をそのまま使用
	getDataEx(_T("Compress"),_T("RawFileName"),&m_cfg.compress.raw_file_name);
	//更新日時を元書庫と同じにする
	getDataEx(_T("Compress"),_T("CopyTimestamp"),&m_cfg.compress.copy_timestamp);

	//解凍
	//ディレクトリを作成する
	getDataEx(_T("Extract"),_T("CreateDir"),&m_cfg.extract.create_dir);
	//二重ディレクトリを防ぐ
	getDataEx(_T("Extract"),_T("DoubleDir"),&m_cfg.extract.create_dir_optimization.remove_redundant_dir.double_dir);
	//ファイル単体の場合作成しない
	getDataEx(_T("Extract"),_T("OnlyFile"),&m_cfg.extract.create_dir_optimization.remove_redundant_dir.only_file);
	//ディレクトリ名末尾の数字を削除
	getDataEx(_T("Extract"),_T("OmitLastNumber"),&m_cfg.extract.create_dir_optimization.omit_number_and_symbol.number);
	//ディレクトリ名末尾の記号を削除
	getDataEx(_T("Extract"),_T("OmitLastSymbol"),&m_cfg.extract.create_dir_optimization.omit_number_and_symbol.symbol);
	//作成するディレクトリの更新日時を書庫と同じにする
	getDataEx(_T("Extract"),_T("CopyTimestamp"),&m_cfg.extract.create_dir_optimization.copy_timestamp);
	//パスワードリストファイル
	getStringDataEx(_T("Extract"),_T("PasswordList"),&m_cfg.extract.password_list_path);
	//ディレクトリのタイムスタンプを復元する
	getDataEx(_T("Extract"),_T("DirTimestamp"),&m_cfg.extract.directory_timestamp);


	//一覧出力
	//FindFirst()/FindNext()/GetFileName()で出力する
	getDataEx(_T("OutputFileList"),_T("APIMode"),&m_cfg.output_file_list.api_mode,true);


	//処理対象フィルタ
	tstring temp_str;

	getStringDataEx(_T("FileFilter"),_T("MinSize"),&temp_str);
	m_cfg.general.filefilter.min_size=_tcstoll(temp_str.c_str(),NULL,10);
	temp_str.clear();
	getStringDataEx(_T("FileFilter"),_T("MaxSize"),&temp_str);
	m_cfg.general.filefilter.max_size=_tcstoll(temp_str.c_str(),NULL,10);
	temp_str.clear();


	m_cfg.general.filefilter.attr=0;

	getStringDataEx(_T("FileFilter"),_T("Attribute"),&temp_str);

	if(!temp_str.empty()){
		int i=0;

		while(temp_str.c_str()[i]!='\0'){
			if(temp_str.c_str()[i]=='e'||temp_str.c_str()[i]=='E'){
				//include_empty_dirは除外フィルタの場合のみ操作
			}else{
				m_cfg.general.filefilter.attr|=str::attr2DWORD(temp_str.c_str()[i]);
			}
			i++;
		}
	}
	temp_str.clear();

	getStringDataEx(_T("FileFilter"),_T("OldestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.filefilter.oldest_date=str::datetime2longlong(temp_str.c_str());
	}
	temp_str.clear();

	getStringDataEx(_T("FileFilter"),_T("NewestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.filefilter.newest_date=str::datetime2longlong(temp_str.c_str(),true);
	}
	temp_str.clear();


	tstring temp_pattern;

	getStringDataEx(_T("FileFilter"),_T("Pattern"),&temp_pattern);
	if(!temp_pattern.empty()){
		//';'で分割
		str::splitString(&m_cfg.general.filefilter.pattern_list,temp_pattern.c_str(),';');
		//重複を削除
		m_cfg.general.filefilter.pattern_list.sort();
		m_cfg.general.filefilter.pattern_list.unique();
		getDataEx(_T("FileFilter"),_T("Recursive"),&m_cfg.general.filefilter.recursive);
	}

	//処理対象外フィルタ
	getStringDataEx(_T("FileExFilter"),_T("MinSize"),&temp_str);
	m_cfg.general.file_ex_filter.min_size=_tcstoll(temp_str.c_str(),NULL,10);
	temp_str.clear();
	getStringDataEx(_T("FileExFilter"),_T("MaxSize"),&temp_str);
	m_cfg.general.file_ex_filter.max_size=_tcstoll(temp_str.c_str(),NULL,10);
	temp_str.clear();


	m_cfg.general.file_ex_filter.attr=0;

	temp_str.clear();

	getStringDataEx(_T("FileExFilter"),_T("Attribute"),&temp_str);

	if(!temp_str.empty()){
		int i=0;

		while(temp_str.c_str()[i]!='\0'){
			if(temp_str.c_str()[i]=='e'||temp_str.c_str()[i]=='E'){
				//include_empty_dirは除外フィルタの場合のみ操作
				m_cfg.general.file_ex_filter.include_empty_dir=false;
			}else{
				m_cfg.general.file_ex_filter.attr|=str::attr2DWORD(temp_str.c_str()[i]);
			}
			i++;
		}
	}

	getStringDataEx(_T("FileExFilter"),_T("OldestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.file_ex_filter.oldest_date=str::datetime2longlong(temp_str.c_str());
	}

	temp_str.clear();

	getStringDataEx(_T("FileExFilter"),_T("NewestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.file_ex_filter.newest_date=str::datetime2longlong(temp_str.c_str(),true);
	}

	temp_pattern.clear();

	getStringDataEx(_T("FileExFilter"),_T("Pattern"),&temp_pattern);
	if(!temp_pattern.empty()){
		//';'で分割
		str::splitString(&m_cfg.general.file_ex_filter.pattern_list,temp_pattern.c_str(),';');
		//重複を削除
		m_cfg.general.file_ex_filter.pattern_list.sort();
		m_cfg.general.file_ex_filter.pattern_list.unique();
		getDataEx(_T("FileExFilter"),_T("Recursive"),&m_cfg.general.file_ex_filter.recursive);
	}


	return true;
}
