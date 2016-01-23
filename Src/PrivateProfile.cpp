//PrivateProfile.cpp
//設定

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r31 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"PrivateProfile.h"

using namespace sslib;


CONFIG Config::m_cfg;
const CONFIG Config::m_default_cfg;
#define CFG_EQUAL(v)(m_cfg.v!=m_default_cfg.v)
#define CFG_VALUE(v)m_cfg.v,(CFG_EQUAL(v))

//PrivateProfileファイルへ書き込む
bool Config::save(){
	//動作モード
	write(_T("Mode"),_T("Mode"),CFG_VALUE(mode));

	//一般
	//バックグラウンドで動作
	write(_T("General"),_T("Background"),CFG_VALUE(general.background_mode));
	//処理終了後ウインドウを閉じる
	write(_T("General"),_T("Quit"),CFG_VALUE(general.quit));
	//ディレクトリ階層を無視して圧縮/解凍
	write(_T("General"),_T("IgnoreDirectoryStructures"),CFG_VALUE(general.ignore_directory_structures));
	//指定したライブラリ名
	write(_T("General"),_T("LibraryName"),CFG_VALUE(general.selected_library_name));
	//指定したライブラリのプレフィックス(msのみ)
	write(_T("General"),_T("LibraryPrefix"),CFG_VALUE(general.selected_library_prefix));
	//出力先ディレクトリ
	write(_T("General"),_T("OutputDir"),CFG_VALUE(general.output_dir));
	//カレントディレクトリ基準で'/od','/of'の相対パスを処理
	write(_T("General"),_T("DefaultBaseDir"),CFG_VALUE(general.default_base_dir));
	//出力ファイルが重複する場合リネーム
	write(_T("General"),_T("AutoRename"),CFG_VALUE(general.auto_rename));
	//ソースを削除
	write(_T("General"),_T("RemoveSource"),CFG_VALUE(general.remove_source));
	//アーカイバに送る文字コード(現在7-zip32.dllのみ対象)
	write(_T("General"),_T("ArcCodePage"),CFG_VALUE(general.arc_codepage));
	//パスワードリストファイル、リストファイルの文字コード
	write(_T("General"),_T("ListCodePage"),CFG_VALUE(general.list_codepage));
	//文字をANSI(sjis)で出力
	write(_T("General"),_T("ANSIstdout"),CFG_VALUE(general.ansi_stdout));
	//ユーザ独自のパラメータ
	write(_T("General"),_T("CustomParam"),CFG_VALUE(general.custom_param));
	//spiがあるディレクトリ
	write(_T("General"),_T("SpiDir"),CFG_VALUE(general.spi_dir));
	//b2eがあるディレクトリ
	write(_T("General"),_T("B2eDir"),CFG_VALUE(general.b2e_dir));
	//wcxがあるディレクトリ
	write(_T("General"),_T("WcxDir"),CFG_VALUE(general.wcx_dir));


	//非表示
	//進捗状況を表示しない
	write(_T("NoDisplay"),_T("Information"),CFG_VALUE(no_display.no_information));
	//ログを表示しない
	write(_T("NoDisplay"),_T("Log"),CFG_VALUE(no_display.no_log));
	//パスワードを表示しない
	write(_T("NoDisplay"),_T("Password"),CFG_VALUE(no_display.no_password));
	//エラーメッセージを表示しない
	write(_T("NoDisplay"),_T("ErrorMessage"),CFG_VALUE(no_display.no_errmsg));


	//再圧縮
	//コマンド実行
	//対話形式で指定
	write(_T("RunCommand"),_T("Interactive"),CFG_VALUE(recompress.run_command.interactive));
	//コマンド
	write(_T("RunCommand"),_T("Command"),CFG_VALUE(recompress.run_command.command));


	//圧縮
	//圧縮形式
	write(_T("Compress"),_T("Type"),m_cfg.compress.compression_type.c_str(),!m_cfg.compress.compression_type.empty()&&!str::isEqualStringIgnoreCase(m_cfg.compress.compression_type,tstring(_T("zip"))));
	//個別圧縮
	write(_T("Compress"),_T("EachFile"),CFG_VALUE(compress.each_file));
	//書庫新規作成
	write(_T("Compress"),_T("CreateNew"),CFG_VALUE(compress.create_new));
	//書庫強制新規作成
	write(_T("Compress"),_T("ForceCreateNew"),CFG_VALUE(compress.force_create_new));
	//基底ディレクトリを含まない
	if(m_cfg.compress.exclude_base_dir<-1)m_cfg.compress.exclude_base_dir=-1;
	write(_T("Compress"),_T("ExcludeBaseDir"),CFG_VALUE(compress.exclude_base_dir));
	//圧縮率
	write(_T("Compress"),_T("Level"),CFG_VALUE(compress.compression_level));
	//書庫分割数値
	write(_T("Compress"),_T("Split"),CFG_VALUE(compress.split_value));
	//出力ファイル名
	write(_T("Compress"),_T("OutputFile"),CFG_VALUE(compress.output_file));
	//'/of'以下の引数をそのまま使用
	write(_T("Compress"),_T("RawFileName"),CFG_VALUE(compress.raw_file_name));
	//更新日時を元書庫と同じにする
	write(_T("Compress"),_T("CopyTimestamp"),CFG_VALUE(compress.copy_timestamp));
	//対象ディレクトリを再帰的検索
	write(_T("Compress"),_T("Recursive"),CFG_VALUE(compress.recursive));
	//b2eスクリプトの圧縮形式
	write(_T("Compress"),_T("B2eFormat"),CFG_VALUE(compress.b2e.format));
	//b2eスクリプトの圧縮メソッド
	write(_T("Compress"),_T("B2eMethod"),CFG_VALUE(compress.b2e.method));
	//b2eスクリプトの自己解凍形式指定
	write(_T("Compress"),_T("B2eSfx"),CFG_VALUE(compress.b2e.sfx));
	//元ファイルの拡張子を保持
	write(_T("Compress"),_T("KeepExtension"),CFG_VALUE(compress.keep_extension));


	//解凍
	//ディレクトリを作成する
	write(_T("Extract"),_T("CreateDir"),CFG_VALUE(extract.create_dir));
	//二重ディレクトリを防ぐ
	write(_T("Extract"),_T("DoubleDir"),CFG_VALUE(extract.create_dir_optimization.remove_redundant_dir.double_dir));
	//同名の二重ディレクトリを防ぐ
	write(_T("Extract"),_T("SameDir"),CFG_VALUE(extract.create_dir_optimization.remove_redundant_dir.same_dir));
	//ファイル単体の場合作成しない
	write(_T("Extract"),_T("OnlyFile"),CFG_VALUE(extract.create_dir_optimization.remove_redundant_dir.only_file));
	//ディレクトリ名末尾の数字を削除
	write(_T("Extract"),_T("OmitLastNumber"),CFG_VALUE(extract.create_dir_optimization.omit_number_and_symbol.number));
	//ディレクトリ名末尾の記号を削除
	write(_T("Extract"),_T("OmitLastSymbol"),CFG_VALUE(extract.create_dir_optimization.omit_number_and_symbol.symbol));
	//作成するディレクトリの更新日時を書庫と同じにする
	write(_T("Extract"),_T("CopyTimestamp"),CFG_VALUE(extract.create_dir_optimization.copy_timestamp));
	//パスワードリストファイル
	write(_T("Extract"),_T("PasswordList"),CFG_VALUE(extract.password_list_path));
	//ディレクトリのタイムスタンプを復元する
	write(_T("Extract"),_T("DirTimestamp"),CFG_VALUE(extract.directory_timestamp));


	//一覧出力
	//FindFirst()/FindNext()/GetFileName()で出力する
	write(_T("OutputFileList"),_T("APIMode"),CFG_VALUE(output_file_list.api_mode));


	//リネーム
	//正規表現
	write(_T("Rename"),_T("Regex"),CFG_VALUE(rename.regex));
	//置換パターン
	if(!m_cfg.rename.pattern_list.empty()||
	   keyExists(_T("Rename"),_T("Pattern"))){
		tstring pattern;

		for(std::list<RENAME::pattern>::const_iterator ite=m_cfg.rename.pattern_list.begin(),
			end=m_cfg.rename.pattern_list.end();
			ite!=end;
			++ite){
			if(ite!=m_cfg.rename.pattern_list.begin())pattern+=_T(";");
			pattern+=ite->first;
			if(!ite->second.empty())pattern+=_T(":")+ite->second;
		}
		write(_T("Rename"),_T("Pattern"),pattern.c_str(),!pattern.empty());
	}


	//処理対象フィルタ
	if(!m_cfg.general.filefilter.empty()||
	   !isEmptySection(_T("FileFilter"))){
		write(_T("FileFilter"),_T("MinSize"),CFG_VALUE(general.filefilter.min_size));
		write(_T("FileFilter"),_T("MaxSize"),CFG_VALUE(general.filefilter.max_size));

		tstring attr_str;

		if(m_cfg.general.filefilter.attr!=m_default_cfg.general.filefilter.attr||
		   keyExists(_T("FileFilter"),_T("Attribute"))){
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_DIRECTORY)attr_str+='d';
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_HIDDEN)attr_str+='h';
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_READONLY)attr_str+='r';
			if(m_cfg.general.filefilter.attr&FILE_ATTRIBUTE_SYSTEM)attr_str+='s';

			write(_T("FileFilter"),_T("Attribute"),attr_str.c_str(),m_cfg.general.filefilter.attr!=m_default_cfg.general.filefilter.attr);
		}

		write(_T("FileFilter"),_T("OldestDate"),strex::longlong2datetime(m_cfg.general.filefilter.oldest_date).c_str(),CFG_EQUAL(general.filefilter.oldest_date));
		write(_T("FileFilter"),_T("NewestDate"),strex::longlong2datetime(m_cfg.general.filefilter.newest_date).c_str(),CFG_EQUAL(general.filefilter.newest_date));

		tstring pattern;

		if(!m_cfg.general.filefilter.pattern_list.empty()||
		   keyExists(_T("FileFilter"),_T("Pattern"))){
			for(std::list<tstring>::const_iterator ite=m_cfg.general.filefilter.pattern_list.begin(),
				end=m_cfg.general.filefilter.pattern_list.end();
				ite!=end;
				++ite){
				if(ite!=m_cfg.general.filefilter.pattern_list.begin())pattern+=_T(";");
				pattern+=*ite;
			}
			write(_T("FileFilter"),_T("Pattern"),pattern.c_str(),!pattern.empty());
		}

		write(_T("FileFilter"),_T("Recursive"),CFG_VALUE(general.filefilter.recursive));
		write(_T("FileFilter"),_T("Regex"),CFG_VALUE(general.filefilter.regex));
	}


	//処理対象外フィルタ
	if(!m_cfg.general.file_ex_filter.empty()||
	   !isEmptySection(_T("FileExFilter"))){
		write(_T("FileExFilter"),_T("MinSize"),CFG_VALUE(general.file_ex_filter.min_size));
		write(_T("FileExFilter"),_T("MaxSize"),CFG_VALUE(general.file_ex_filter.max_size));


		if(m_cfg.general.file_ex_filter.attr!=m_default_cfg.general.file_ex_filter.attr||
		   keyExists(_T("FileExFilter"),_T("Attribute"))){
			tstring attr_str;

			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_DIRECTORY)attr_str+='d';
			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_HIDDEN)attr_str+='h';
			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_READONLY)attr_str+='r';
			if(m_cfg.general.file_ex_filter.attr&FILE_ATTRIBUTE_SYSTEM)attr_str+='s';

			write(_T("FileExFilter"),_T("Attribute"),attr_str.c_str(),m_cfg.general.file_ex_filter.attr!=m_default_cfg.general.file_ex_filter.attr);
		}

		write(_T("FileExFilter"),_T("IncludeEmptyDir"),CFG_VALUE(general.file_ex_filter.include_empty_dir));

		write(_T("FileExFilter"),_T("OldestDate"),strex::longlong2datetime(m_cfg.general.file_ex_filter.oldest_date).c_str(),CFG_EQUAL(general.file_ex_filter.oldest_date));
		write(_T("FileExFilter"),_T("NewestDate"),strex::longlong2datetime(m_cfg.general.file_ex_filter.newest_date).c_str(),CFG_EQUAL(general.file_ex_filter.newest_date));


		if(m_cfg.general.file_ex_filter.pattern_list!=m_default_cfg.general.file_ex_filter.pattern_list||
		   keyExists(_T("FileExFilter"),_T("Pattern"))){
			tstring pattern;

			for(std::list<tstring>::const_iterator ite=m_cfg.general.file_ex_filter.pattern_list.begin(),
				end=m_cfg.general.file_ex_filter.pattern_list.end();
				ite!=end;
				++ite){
				if(ite!=m_cfg.general.file_ex_filter.pattern_list.begin())pattern+=_T(";");
				pattern+=*ite;
			}
			write(_T("FileExFilter"),_T("Pattern"),pattern.c_str(),!pattern.empty());
		}

		write(_T("FileExFilter"),_T("Recursive"),CFG_VALUE(general.file_ex_filter.recursive));
		write(_T("FileExFilter"),_T("Regex"),CFG_VALUE(general.file_ex_filter.regex));
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
	//アーカイバに送る文字コード(現在7-zip32.dllのみ対象)
	getDataEx(_T("General"),_T("ArcCodePage"),&m_cfg.general.arc_codepage);
	//パスワードリストファイル、リストファイルの文字コード
	getDataEx(_T("General"),_T("ListCodePage"),&m_cfg.general.list_codepage);
	//文字をANSI(sjis)で出力
	getDataEx(_T("General"),_T("ANSI"),&m_cfg.general.ansi_stdout);
	//ユーザ独自のパラメータ
	getStringDataEx(_T("General"),_T("CustomParam"),&m_cfg.general.custom_param);
	//spiのあるディレクトリ
	getStringDataEx(_T("General"),_T("SpiDir"),&m_cfg.general.spi_dir);
	//b2eのあるディレクトリ
	getStringDataEx(_T("General"),_T("B2eDir"),&m_cfg.general.b2e_dir);
	//wcxのあるディレクトリ
	getStringDataEx(_T("General"),_T("WcxDir"),&m_cfg.general.wcx_dir);


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
	//書庫新規作成
	getDataEx(_T("Compress"),_T("CreateNew"),&m_cfg.compress.create_new);
	//書庫強制新規作成
	getDataEx(_T("Compress"),_T("ForceCreateNew"),&m_cfg.compress.force_create_new);
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
	//対象ディレクトリを再帰的検索
	getDataEx(_T("Compress"),_T("Recursive"),&m_cfg.compress.recursive);
	//b2eスクリプトの圧縮形式
	getStringDataEx(_T("Compress"),_T("B2eFormat"),&m_cfg.compress.b2e.format);
	//b2eスクリプトの圧縮メソッド
	getStringDataEx(_T("Compress"),_T("B2eMethod"),&m_cfg.compress.b2e.method);
	//b2eスクリプトの自己解凍形式指定
	getDataEx(_T("Compress"),_T("B2eSfx"),&m_cfg.compress.b2e.sfx);
	//元ファイルの拡張子を保持
	getDataEx(_T("Compress"),_T("KeepExtension"),&m_cfg.compress.keep_extension);

	//解凍
	//ディレクトリを作成する
	getDataEx(_T("Extract"),_T("CreateDir"),&m_cfg.extract.create_dir);
	//二重ディレクトリを防ぐ
	getDataEx(_T("Extract"),_T("DoubleDir"),&m_cfg.extract.create_dir_optimization.remove_redundant_dir.double_dir);
	//同名の二重ディレクトリを防ぐ
	getDataEx(_T("Extract"),_T("SameDir"),&m_cfg.extract.create_dir_optimization.remove_redundant_dir.same_dir);
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


	tstring temp_str;
	tstring temp_pattern;

	//リネーム
	//置換パターン
	getStringDataEx(_T("Rename"),_T("Pattern"),&temp_pattern);
	if(!temp_pattern.empty()){
		std::list<tstring> pattern_list;
		//';'で分割
		str::splitString(&pattern_list,temp_pattern.c_str(),';');
		//重複を削除
		misc::undupList(&m_cfg.general.filefilter.pattern_list);

		for(std::list<tstring>::const_iterator ite=pattern_list.begin(),
			end=pattern_list.end();
			ite!=end;++ite){
			std::list<tstring> pattern;

			str::splitString(&pattern,ite->c_str(),':');
			if(pattern.front().empty())continue;
			if(pattern.size()<2)pattern.push_back(_T(""));
			m_cfg.rename.pattern_list.push_back(RENAME::pattern(pattern.front(),pattern.back()));
		}

		//正規表現
		getDataEx(_T("Rename"),_T("Regex"),&m_cfg.rename.regex);
	}
	temp_pattern.clear();


	//処理対象フィルタ
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
				m_cfg.general.filefilter.attr|=strex::attr2DWORD(temp_str.c_str()[i]);
			}
			i++;
		}
	}
	temp_str.clear();

	getStringDataEx(_T("FileFilter"),_T("OldestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.filefilter.oldest_date=strex::datetime2longlong(temp_str.c_str());
	}
	temp_str.clear();

	getStringDataEx(_T("FileFilter"),_T("NewestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.filefilter.newest_date=strex::datetime2longlong(temp_str.c_str(),true);
	}
	temp_str.clear();

	getStringDataEx(_T("FileFilter"),_T("Pattern"),&temp_pattern);
	if(!temp_pattern.empty()){
		//';'で分割
		str::splitString(&m_cfg.general.filefilter.pattern_list,temp_pattern.c_str(),';');
		//重複を削除
		misc::undupList(&m_cfg.general.filefilter.pattern_list);
		getDataEx(_T("FileFilter"),_T("Recursive"),&m_cfg.general.filefilter.recursive);
		getDataEx(_T("FileFilter"),_T("Regex"),&m_cfg.general.filefilter.regex);
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
				m_cfg.general.file_ex_filter.attr|=strex::attr2DWORD(temp_str.c_str()[i]);
			}
			i++;
		}
	}
	temp_str.clear();

	getDataEx(_T("FileExFilter"),_T("IncludeEmptyDir"),&m_cfg.general.file_ex_filter.include_empty_dir);

	getStringDataEx(_T("FileExFilter"),_T("OldestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.file_ex_filter.oldest_date=strex::datetime2longlong(temp_str.c_str());
	}

	temp_str.clear();

	getStringDataEx(_T("FileExFilter"),_T("NewestDate"),&temp_str);
	if(!temp_str.empty()){
		m_cfg.general.file_ex_filter.newest_date=strex::datetime2longlong(temp_str.c_str(),true);
	}

	temp_pattern.clear();

	getStringDataEx(_T("FileExFilter"),_T("Pattern"),&temp_pattern);
	if(!temp_pattern.empty()){
		//';'で分割
		str::splitString(&m_cfg.general.file_ex_filter.pattern_list,temp_pattern.c_str(),';');
		//重複を削除
		misc::undupList(&m_cfg.general.file_ex_filter.pattern_list);
		getDataEx(_T("FileExFilter"),_T("Recursive"),&m_cfg.general.file_ex_filter.recursive);
		getDataEx(_T("FileExFilter"),_T("Regex"),&m_cfg.general.file_ex_filter.regex);
	}


	return true;
}
