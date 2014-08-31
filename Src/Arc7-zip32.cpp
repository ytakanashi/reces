//Arc7zip32.cpp
//7-zip32.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r22 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"Arc7-zip32.h"
#include"Hook/HookArchiverDialog.h"

using namespace sslib;



Arc7zip32::Arc7zip32(ArcCfg& arc_cfg):
	ArcDll(_T("7-zip32"),
			_T("SevenZip"),
			_T("zip.7z.jar"),
			_T("\\")),
	m_arc_cfg(arc_cfg){
		COMPRESSION_METHOD method[]={
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
		m_compression_methods.assign(method,method+ARRAY_SIZEOF(method));
}

//対応している書庫であるか
bool Arc7zip32::isSupportedArchive(const TCHAR* arc_path_orig,const DWORD mode){
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

#if SEVENZIP_OLD_VERSION
	//7-zip32 v.9.20.00.02で
	//ウインドウを持たないプロセスが暗号化書庫に対し
	//CheckArchive(CHECKARCHIVE_BASIC)すると
	//falseを返す不具合対策
	sslib::Window dummy_window(::GetModuleHandle(NULL),NULL,false,true);

	dummy_window.registerWindow(_T("_DUMMY_WINDOW_"),0);
	dummy_window.createWindow(0,0,NULL,0,0,0,0);
#endif

	bool use_password=!m_arc_cfg.cfg().general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=m_arc_cfg.cfg().general.password_list.begin();
	std::list<tstring>::iterator password_list_end=m_arc_cfg.cfg().general.password_list.end();

	do{
		if(use_password&&ite_password_list!=password_list_end){
			m_arc_cfg.cfg().general.password=*ite_password_list;
			++ite_password_list;
		}else{
			m_arc_cfg.cfg().general.password.clear();
		}
		result=checkArchive(arc_path.c_str(),CHECKARCHIVE_BASIC);
	}while(!isTerminated()&&
		   !m_arc_cfg.m_password_input_cancelled&&
		   //正しいパスワードが入力されるまで問い合わせる
		   !result&&
		   m_arc_cfg.m_hook_dialog_type==HOOK_7ZIP32_PASSWORD);

	if(result&&
	   !m_arc_cfg.cfg().general.password.empty()){
		//ここで設定しないとOpenArchive()が失敗してしまう?
		setDefaultPassword(m_arc_cfg.cfg().general.password.c_str());
	}

	return result;
}

//書庫をテスト
bool Arc7zip32::test(const TCHAR* arc_path_orig,tstring* log_msg){
	tstring arc_path(arc_path_orig);

	replaceDelimiter(arc_path);

	VariableArgument cmd_line(_T("%s %s %s %s%s%s"),
							  _T("t"),
							  _T("-hide"),
							  _T("--"),
							  (str::containsWhiteSpace(arc_path))?_T("\""):_T(""),
							  arc_path.c_str(),
							  (str::containsWhiteSpace(arc_path))?_T("\""):_T(""));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

	bool use_password=!m_arc_cfg.cfg().general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=m_arc_cfg.cfg().general.password_list.begin();
	std::list<tstring>::iterator password_list_end=m_arc_cfg.cfg().general.password_list.end();

	int dll_ret=-1;
	if(!m_arc_cfg.cfg().no_display.no_information&&
	   !app()->stdOut().isRedirected()){
		m_processing_info.clear();
	}

	do{
		if(use_password&&ite_password_list!=password_list_end){
			m_arc_cfg.cfg().general.password=*ite_password_list;
			++ite_password_list;
		}else{
			m_arc_cfg.cfg().general.password.clear();
		}

		if(log_msg==NULL){
			//実行
			tstring dummy(1,'\0');

			dll_ret=execute(NULL,cmd_line.get(),&dummy,dummy.length());
		}else{
			dll_ret=execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
		}
	}while(!isTerminated()&&
		   //正しいパスワードが入力されるまで問い合わせる
		   dll_ret==ERROR_PASSWORD_FILE);

	if(!m_arc_cfg.cfg().no_display.no_information)app()->stdOut().outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	return dll_ret==0;
}

//圧縮形式を取得(その形式に対応している場合のみ)
tstring Arc7zip32::getCompressionMethod(const TCHAR* arc_path_orig){
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

ArcDll::ARCDLL_RESULT Arc7zip32::compress(const TCHAR* arc_path_orig,std::list<tstring>* file_list,tstring* log_msg){
	if(getMethod().mhd==NULL)return ARCDLL_FAILURE;

	tstring arc_path(arc_path_orig);

	m_file_size=m_write_size=0;

	//階層無視圧縮ではリスト出力が必要!
	if(m_arc_cfg.cfg().general.ignore_directory_structures&&
	   (m_arc_cfg.cfg().general.filefilter.empty()&&m_arc_cfg.cfg().general.file_ex_filter.empty())){
		m_arc_cfg.cfg().general.filefilter.pattern_list.push_back(tstring(_T("*")));
	}

	bool use_filter=!m_arc_cfg.cfg().general.filefilter.empty()||!m_arc_cfg.cfg().general.file_ex_filter.empty();
	bool use_exclude_list=false;

	//リストファイルにパスを出力
	File list_file;
	tstring list_file_path;
	File exclude_list_file;
	tstring exclude_list_file_path;

	if(m_arc_cfg.cfg().mode==MODE_RECOMPRESS){
		//リストファイルを作成
		list_file_path=fileoperation::createTempFile(_T("7z"),m_arc_cfg.m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARCDLL_CANNOT_OPEN_LISTFILE;
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
				if(!m_arc_cfg.cfg().general.ignore_directory_structures&&
				   use_filter){
					exclude_list_file_path=fileoperation::createTempFile(_T("7z"),m_arc_cfg.m_list_temp_dir.c_str());
					if(!exclude_list_file.open(exclude_list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
						return ARCDLL_CANNOT_OPEN_LISTFILE;
					}
				}

				//リストファイルを作成
				list_file_path=fileoperation::createTempFile(_T("7z"),m_arc_cfg.m_list_temp_dir.c_str());
				if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
					return ARCDLL_CANNOT_OPEN_LISTFILE;
				}
			}

			//リストを出力(フィルタが有効であれば処理対象外リストも出力)
			writeFormatedList(list_file,exclude_list_file,*ite);

			//区切り文字置換
			replaceDelimiter(list_file_path);
		}

		if(list_file.getSize()==0){
			return ARCDLL_NO_MATCHES_FOUND;
		}

		if(!m_arc_cfg.cfg().general.ignore_directory_structures&&
		   use_filter){
			use_exclude_list=exclude_list_file.getSize()!=0;
			exclude_list_file.close();
		}
	}

	exclude_list_file.close();
	list_file.close();
	replaceDelimiter(exclude_list_file_path);
	replaceDelimiter(list_file_path);

	//圧縮率の処理
	tstring level_str;

	if(m_arc_cfg.cfg().compress.compression_level==minimum_compressionlevel){
		//'/l'と指定された場合
		m_arc_cfg.cfg().compress.compression_level=getMethod().minimum_level;
	}else if(m_arc_cfg.cfg().compress.compression_level==maximum_compressionlevel){
		//'/lx'と指定された場合
		m_arc_cfg.cfg().compress.compression_level=getMethod().maximum_level;
	}

	if(m_arc_cfg.cfg().compress.compression_level!=-1){
		//圧縮率を範囲内に収める
		m_arc_cfg.cfg().compress.compression_level=clamp(m_arc_cfg.cfg().compress.compression_level,
											   getMethod().minimum_level,
											   getMethod().maximum_level);

		level_str=getMethod().level;
		level_str+=static_cast<TCHAR>(m_arc_cfg.cfg().compress.compression_level+'0');
	}


	//区切り文字置換
	replaceDelimiter(arc_path);
	replaceDelimiter(&m_arc_cfg.cfg().general.filefilter.pattern_list);
	replaceDelimiter(&m_arc_cfg.cfg().general.file_ex_filter.pattern_list);
	replaceDelimiter(file_list);

	//勝手に拡張子が付加されないように'.'をファイル名末尾に追加。
	arc_path+=_T(".");

	if(m_arc_cfg.cfg().mode==MODE_RECOMPRESS){
		//再圧縮する書庫のパスワードを新たに設定
		if(!m_arc_cfg.cfg().recompress.new_password.empty()){
			m_arc_cfg.cfg().general.password=m_arc_cfg.cfg().recompress.new_password;
		}
	}

	if(!m_arc_cfg.cfg().no_display.no_information)app()->stdOut().outputString(_T("'%s'を処理しています...\n\n"),arc_path_orig);

	VariableArgument cmd_line(_T("%s %s %s %s %s %s "),
							  _T("a"),
							  (m_arc_cfg.cfg().compress.create_new)?_T("-y -mmt=on -up0q0"):_T("-y -mmt=on"),
							  //-y     : 全ての質問に yes を仮定。
							  //-mmt=on: マルチスレッドモードを設定。
							  //-u     :
							  //  p    :ファイルは書庫内に存在するがワイルドカード名と一致しない。
							  //  q    :ファイルは書庫内に存在するがディスク上には存在しない。
							  getMethod().cmd,
							  //getMethod().level+compression_level
							  (m_arc_cfg.cfg().compress.compression_level!=-1&&!level_str.empty())?level_str.c_str():_T(""),
							  m_arc_cfg.cfg().general.custom_param.c_str(),
							  _T("-hide"));

	if(use_exclude_list){
		//ディレクトリを/mcで圧縮(with フィルタ)する場合
		//-xスイッチで処理対象外リストを指定
		cmd_line.add(_T("-x@%s%s%s "),
					 (str::containsWhiteSpace(exclude_list_file_path))?_T("\""):_T(""),
					 exclude_list_file_path.c_str(),
					 (str::containsWhiteSpace(exclude_list_file_path))?_T("\""):_T(""));
	}

	cmd_line.add(_T("%s %s%s%s @%s%s%s"),
				 _T("--"),

				 (str::containsWhiteSpace(arc_path))?_T("\""):_T(""),
				 arc_path.c_str(),
				 (str::containsWhiteSpace(arc_path))?_T("\""):_T(""),

				 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
				 list_file_path.c_str(),
				 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

	//実行
	int dll_ret=-1;
	if(!m_arc_cfg.cfg().no_display.no_information&&
	   !app()->stdOut().isRedirected()){
		m_processing_info.clear();
	}

	if(m_arc_cfg.cfg().no_display.no_log||log_msg==NULL){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.get(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
	}

	if(!m_arc_cfg.cfg().no_display.no_information)app()->stdOut().outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	unload();

	return (dll_ret==0)?ARCDLL_SUCCESS:ARCDLL_FAILURE;
}

ArcDll::ARCDLL_RESULT Arc7zip32::extract(const TCHAR* arc_path_orig,const TCHAR* output_dir_orig,tstring* log_msg){
	tstring arc_path(arc_path_orig);
	tstring output_dir(output_dir_orig);

	tstring list_file_path;
	File list_file;

	if(m_arc_cfg.cfg().general.ignore_directory_structures){
		//7-zip32系のライブラリは'e'解凍でディレクトリを作成してしまうので、
		//解凍対象ファイルをリストファイルに書き出して、作成を抑制する
		if(m_arc_cfg.cfg().general.filefilter.empty()){
			m_arc_cfg.cfg().general.filefilter.pattern_list.push_back(_T("*"));
		}
	}

	bool use_filter=!m_arc_cfg.cfg().general.filefilter.empty()||!m_arc_cfg.cfg().general.file_ex_filter.empty();

	if(use_filter){
		//リストファイルを作成
		list_file_path=fileoperation::createTempFile(_T("7z"),m_arc_cfg.m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARCDLL_CANNOT_OPEN_LISTFILE;
		}

		//リストファイルに解凍対象ファイルのみ出力
		outputFileListEx(arc_path.c_str(),
							 m_arc_cfg.cfg().general.filefilter,
							 m_arc_cfg.cfg().general.file_ex_filter,
							 list_file,
							 //フィルタ適用を逆にする
							 (!m_arc_cfg.cfg().general.ignore_directory_structures)?REVERSE_FILTER:0);
		list_file.close();
	}

	tstring output_dir_bak;
	fileoperation::scheduleDelete schedule_delete;

	if(m_arc_cfg.cfg().compress.exclude_base_dir!=0){
		output_dir_bak=output_dir;
		output_dir=path::addTailSlash(fileoperation::createTempDir(_T("rcs"),output_dir.c_str()));

		//一時ディレクトリ削除予約
		schedule_delete.set(output_dir.c_str());
		//削除漏れ対策
		m_arc_cfg.m_schedule_list.push_back(new fileoperation::scheduleDelete(output_dir.c_str()));
	}

	//区切り文字置換
	replaceDelimiter(arc_path);
	replaceDelimiter(output_dir);

	VariableArgument cmd_line(_T("%s %s %s %s "),
							  (!m_arc_cfg.cfg().general.ignore_directory_structures)?_T("x"):_T("e"),
							  _T("-y -aoa -mmt=on"),
							  //-y     : 全ての質問に yes を仮定。
							  //-aoa   : 全てのファイルを確認しないで上書きします。
							  //-mmt=on: マルチスレッドモードを設定。
							  m_arc_cfg.cfg().general.custom_param.c_str(),
							  _T("-hide"));

	if(use_filter&&
	   !m_arc_cfg.cfg().general.ignore_directory_structures){
		//通常解凍(with フィルタ)する場合
		//-xスイッチで処理対象外リストを指定
		cmd_line.add(_T("-x@%s%s%s "),
					 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
					 list_file_path.c_str(),
					 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""));
	}

	cmd_line.add(_T("-o%s%s%s %s %s%s%s"),
				 (str::containsWhiteSpace(output_dir))?_T("\""):_T(""),
				 output_dir.c_str(),
				 (str::containsWhiteSpace(output_dir))?_T("\""):_T(""),

				 _T("--"),

				 (str::containsWhiteSpace(arc_path))?_T("\""):_T(""),
				 arc_path.c_str(),
				 (str::containsWhiteSpace(arc_path))?_T("\""):_T(""));

	if(use_filter&&
	   m_arc_cfg.cfg().general.ignore_directory_structures){
		cmd_line.add(_T(" @%s%s%s"),
					 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
					 list_file_path.c_str(),
					 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""));
	}

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

	if(!m_arc_cfg.cfg().no_display.no_information)app()->stdOut().outputString(_T("'%s'を処理しています...\n\n"),arc_path_orig);

	bool use_password=!m_arc_cfg.cfg().general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=m_arc_cfg.cfg().general.password_list.begin();
	std::list<tstring>::iterator password_list_end=m_arc_cfg.cfg().general.password_list.end();

	int dll_ret=-1;
	if(!m_arc_cfg.cfg().no_display.no_information&&
	   !app()->stdOut().isRedirected()){
		m_processing_info.clear();
	}

	do{
		if(use_password){
			m_arc_cfg.cfg().general.password=*ite_password_list;
		}

		//実行
		if(m_arc_cfg.cfg().no_display.no_log||log_msg==NULL){
			tstring dummy(1,'\0');

			dll_ret=execute(NULL,cmd_line.get(),&dummy,dummy.length());
		}else{
			dll_ret=execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
		}
	}while(use_password&&
		   dll_ret!=0&&
		   (++ite_password_list)!=password_list_end);

	if(!m_arc_cfg.cfg().no_display.no_information)app()->stdOut().outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	//パス区切り文字を'\\'に
	if(*m_delimiter=='/')str::replaceCharacter(output_dir,'/','\\');

	if(m_arc_cfg.cfg().general.decode_uesc){
		//ファイル名に含まれるUnicodeエスケープシーケンスをデコードする
		decodeUnicodeEscape(arc_path_orig,output_dir.c_str(),m_arc_cfg.cfg().general.ignore_directory_structures);
	}

	if(!m_arc_cfg.cfg().general.ignore_directory_structures&&
	   m_arc_cfg.cfg().extract.directory_timestamp){
		//ディレクトリの更新日時を復元
		recoverDirectoryTimestamp(arc_path_orig,output_dir.c_str(),m_arc_cfg.cfg().general.decode_uesc,true);
	}

	unload();

	if(m_arc_cfg.cfg().compress.exclude_base_dir!=0){
		//共通パスを取り除く
		excludeCommonPath(output_dir_bak.c_str(),output_dir.c_str(),m_arc_cfg.cfg().compress.exclude_base_dir);
	}

	return (dll_ret==0)?ARCDLL_SUCCESS:ARCDLL_FAILURE;
}

ArcDll::ARCDLL_RESULT Arc7zip32::del(const TCHAR* arc_path_orig,tstring* log_msg){
	tstring arc_path(arc_path_orig);

	tstring list_file_path;
	File list_file;
	bool use_filter=!m_arc_cfg.cfg().general.filefilter.empty()||!m_arc_cfg.cfg().general.file_ex_filter.empty();

	if(!use_filter||
	   !m_arc_cfg.cfg().recompress.run_command.disable()||
	   m_arc_cfg.cfg().general.ignore_directory_structures||
	   m_arc_cfg.cfg().compress.compression_level!=-1){
		return ARCDLL_FAILURE;
	}

	//リストファイルを作成
	list_file_path=fileoperation::createTempFile(_T("7z"),m_arc_cfg.m_list_temp_dir.c_str());
	if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
		return ARCDLL_CANNOT_OPEN_LISTFILE;
	}

	//リストファイルに解凍対象ファイルのみ出力
	outputFileListEx(arc_path.c_str(),
						 m_arc_cfg.cfg().general.filefilter,
						 m_arc_cfg.cfg().general.file_ex_filter,
						 list_file,
						 //フィルタ適用を逆にする
						 REVERSE_FILTER);
	list_file.close();

	//区切り文字置換
	replaceDelimiter(arc_path);
	replaceDelimiter(list_file_path);

	//勝手に拡張子が付加されないように'.'をファイル名末尾に追加。
	arc_path+=_T(".");

	VariableArgument cmd_line(_T("%s %s %s -t%s %s %s%s%s @%s%s%s"),
							  _T("d"),
							  _T("-y -mmt=on"),
							  //-y     : 全ての質問に yes を仮定。
							  //-mmt=on: マルチスレッドモードを設定。
							  _T("-hide"),

							  //書庫形式指定
							  getCompressionMethod(arc_path.c_str()).c_str(),

							  _T("--"),

							  (str::containsWhiteSpace(arc_path))?_T("\""):_T(""),
							  arc_path.c_str(),
							  (str::containsWhiteSpace(arc_path))?_T("\""):_T(""),

							  (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
							  list_file_path.c_str(),
							  (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

	if(!m_arc_cfg.cfg().no_display.no_information)app()->stdOut().outputString(_T("'%s'を処理しています...\n\n"),arc_path_orig);

	//実行
	int dll_ret=-1;
	if(!m_arc_cfg.cfg().no_display.no_information&&
	   !app()->stdOut().isRedirected()){
		m_processing_info.clear();
	}

	if(m_arc_cfg.cfg().no_display.no_log||log_msg==NULL){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.get(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
	}

	if(!m_arc_cfg.cfg().no_display.no_information)app()->stdOut().outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	unload();

	return (dll_ret==0)?ARCDLL_SUCCESS:ARCDLL_FAILURE;
}

void Arc7zip32::list(const TCHAR* arc_path_orig,tstring* log_msg){
	if(log_msg==NULL)return;

	tstring arc_path(arc_path_orig);

	replaceDelimiter(arc_path);

	//区切り文字置換
	replaceDelimiter(arc_path);

	if(m_arc_cfg.cfg().output_file_list.api_mode){
//		setDefaultPassword(m_arc_cfg.cfg().general.password.c_str());
		outputFileListEx(arc_path.c_str(),m_arc_cfg.cfg().general.filefilter,m_arc_cfg.cfg().general.file_ex_filter,(m_arc_cfg.cfg().general.decode_uesc)?DECODE_UNICODE_ESCAPE:0);
//		setDefaultPassword(NULL);
	}else{
		tstring list_file_path;
		File list_file;

		bool use_filter=!m_arc_cfg.cfg().general.filefilter.empty()||!m_arc_cfg.cfg().general.file_ex_filter.empty();

		if(use_filter){
			//リストファイルを作成
			list_file_path=fileoperation::createTempFile(_T("7z"),m_arc_cfg.m_list_temp_dir.c_str());
			if(list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
				//リストファイルに列挙対象ファイルのみ出力
				outputFileListEx(arc_path.c_str(),
									 m_arc_cfg.cfg().general.filefilter,
									 m_arc_cfg.cfg().general.file_ex_filter,
									 list_file,
									 //フィルタ適用を逆にする
									 REVERSE_FILTER);
				list_file.close();
			}else{
				use_filter=false;
			}
		}

		VariableArgument cmd_line(_T("%s %s "),
								  _T("l"),
								  _T("-hide"));

		if(use_filter){
			//-xスイッチで処理対象外リストを指定
			cmd_line.add(_T("-x@%s%s%s "),
						 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
						 list_file_path.c_str(),
						 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""));
		}

		cmd_line.add(_T("%s %s%s%s"),
					 _T("--"),
					 (str::containsWhiteSpace(arc_path))?_T("\""):_T(""),
					 arc_path.c_str(),
					 (str::containsWhiteSpace(arc_path))?_T("\""):_T(""));

		dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

		execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
	}
}

//リストにフィルタを適用
void Arc7zip32::applyFilters(std::list<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse){
	if(filefilter.empty()&&file_ex_filter.empty())return;

	//除外リストを作成
	std::list<fileinfo::FILEINFO> exclude_list;
	FileTree file_tree(filefilter,file_ex_filter);

	fileinfo_list->sort();

	for(std::list<fileinfo::FILEINFO>::iterator ite=fileinfo_list->begin(),
		end=fileinfo_list->end();
		ite!=end;
		++ite){
		tstring parent_dir(path::addTailSlash(path::getParentDirectory(ite->name)));
		if(parent_dir!=ite->name){
			file_tree.add(parent_dir.c_str(),ite->name.c_str(),&*ite);
		}
	}

	if(reverse){
		file_tree.makeExcludeTree(FileTree::TO_NONE,_T(""));
	}else{
		file_tree.makeIncludeTree(FileTree::TO_NONE,_T(""));
	}
	fileinfo_list->clear();
	file_tree.tree2list(*fileinfo_list);

	if(m_arc_cfg.cfg().general.ignore_directory_structures){
		for(std::list<fileinfo::FILEINFO>::iterator ite=fileinfo_list->begin(),
			end=fileinfo_list->end();
			ite!=end;){
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
	return list_file.writeEx(_T("%s%s%s%s%s\r\n"),
							(str::containsWhiteSpace(base_dir)||str::containsWhiteSpace(file_path))?_T("\""):_T(""),
							base_dir,
							m_delimiter,
							file_path,
							(str::containsWhiteSpace(base_dir)||str::containsWhiteSpace(file_path))?_T("\""):_T(""));
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD Arc7zip32::writeFormatedPath(const File& list_file,const TCHAR* file_path){
	return list_file.writeEx(_T("%s%s%s\r\n"),
							(str::containsWhiteSpace(file_path))?_T("\""):_T(""),
							file_path,
							(str::containsWhiteSpace(file_path))?_T("\""):_T(""));
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool Arc7zip32::writeFormatedList(const File& list_file,const tstring& full_path){
	return false;
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool Arc7zip32::writeFormatedList(const File& list_file,File& exclude_list_file,const tstring& full_path){
	bool use_filter=!m_arc_cfg.cfg().general.filefilter.empty()||!m_arc_cfg.cfg().general.file_ex_filter.empty();
	bool use_exclude_list=false;

	tstring base_dir(path::getParentDirectory(full_path));
	tstring file_name(path::getFileName(full_path));

	if(path::isDirectory(full_path.c_str())){
		//full_pathはディレクトリ

		//フィルタ使用時のみ使用
		std::list<size_info> relative_path_list;

		if(use_filter){
			//フィルタ使用時
			std::list<fileinfo::FILEINFO> filtered_list;
			FileTree file_tree(m_arc_cfg.cfg().general.filefilter,m_arc_cfg.cfg().general.file_ex_filter);

			file_tree.createFileTree(full_path.c_str(),_T("*"),true);

			if(!m_arc_cfg.cfg().general.ignore_directory_structures){
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
				for(std::list<fileinfo::FILEINFO>::iterator ite=filtered_list.begin(),
					end=filtered_list.end();
					ite!=end;
					++ite){
					if(use_exclude_list){
						//処理対象外リストは[親ディレクトリ基準の]相対パス
						///eb時は[対象ディレクトリ基準の]相対パス
						relative_path_list.push_back(size_info(
							path::makeRelativePath((m_arc_cfg.cfg().compress.exclude_base_dir==0)?
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

		//処理対象リストに書き出し
		if(m_arc_cfg.cfg().compress.exclude_base_dir==0){
			writeFormatedPath(list_file,full_path.c_str());
		}else{
			//'C:\~\DIR\*'
			file_name=path::addTailSlash(full_path);
			file_name+=_T("*");
			writeFormatedPath(list_file,file_name.c_str());
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
				if(!m_arc_cfg.cfg().general.ignore_directory_structures){
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
			if(!m_arc_cfg.cfg().general.ignore_directory_structures){
				m_file_size+=fileoperation::getDirectorySize(full_path.c_str())-exclude_file_size;
			}
		}
	}else{

		//full_pathはファイル
		if(fileinfo::matchFilters(full_path.c_str(),m_arc_cfg.cfg().general.filefilter,m_arc_cfg.cfg().general.file_ex_filter,base_dir.c_str())){
			if(!m_arc_cfg.cfg().general.ignore_directory_structures){
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
