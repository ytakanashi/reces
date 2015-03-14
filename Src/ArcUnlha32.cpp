//ArcUnlha32.cpp
//Unlha32.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r26 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcUnlha32.h"
#include"ArcCfg.h"
#include"FileInfo.h"
#include"PrivateProfile.h"

using namespace sslib;



ArcUnlha32::ArcUnlha32():
	ArcDll(_T("Unlha32"),
			_T("Unlha"),
			_T("lzh.lha.lzs"),
			_T("/")),
	m_file_size(0),
	m_write_size(0),
	m_last_write_size(0),
	m_last_hash(0){
		COMPRESSION_METHOD method[]={
			{_T("lzh"),_T(".lzh"),_T(""),_T(""),
				0,
				-1,-1,-1},
			{_T("lzhsfx"),_T(".exe"),_T("-jw1"),_T(""),
				MHD_SFX,
				-1,-1,-1},
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_methods.assign(method,method+ARRAY_SIZEOF(method));
}

//書庫をテスト
ArcUnlha32::ARC_RESULT ArcUnlha32::test(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	tstring cmd_line(format(_T("%s %s %s"),
							_T("t"),
							_T("-n1"),
							quotePath(arc_path_str).c_str()));

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

ArcUnlha32::ARC_RESULT ArcUnlha32::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	if(m_compression_methods[m_method_index].mhd==NULL)return ARC_FAILURE;

	tstring arc_path_str(arc_path);

	m_file_size=m_write_size=m_last_write_size=m_last_hash=0;

	//階層無視圧縮ではリスト出力が必要!
	if(CFG.general.ignore_directory_structures&&
	   (CFG.general.filefilter.empty()&&CFG.general.file_ex_filter.empty())){
		CFG.general.filefilter.pattern_list.push_back(tstring(_T("*")));
	}

	tstring list_file_path;
	File list_file;

	//リストファイルを作成
	list_file_path=tempfile::create(_T("lzh"),ARCCFG->m_list_temp_dir.c_str());
	if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
		return ARC_CANNOT_OPEN_LISTFILE;
	}

	//リストファイルにパスを出力
	if(CFG.mode==MODE_RECOMPRESS){
		list_file.writeEx(_T("*\r\n"));

		//ファイルサイズ計算
		if(!CFG.no_display.no_information&&
		   !STDOUT.isRedirected()){
			m_file_size=fileoperation::getDirectorySize(path::getCurrentDirectory().c_str());
		}
	}else{
		for(std::list<tstring>::const_iterator ite=file_list->begin(),
			end=file_list->end();
			ite!=end;
			++ite){
			writeFormatedList(list_file,*ite);
		}
	}

	if(!list_file.getSize())return ARC_NO_MATCHES_FOUND;

	list_file.close();

	//区切り文字置換
	replaceDelimiter(arc_path_str);
	replaceDelimiter(&CFG.general.filefilter.pattern_list);
	replaceDelimiter(&CFG.general.file_ex_filter.pattern_list);
	replaceDelimiter(file_list);
	replaceDelimiter(list_file_path);

	//勝手に拡張子が付加されないように'.'をファイル名末尾に追加。
	if(lstrcmp(getMethod().mhd,_T("lzhsfx"))!=0)arc_path_str+=_T(".");

	tstring cmd_line(format(_T("%s %s %s %s %s %s @%s"),
									  _T("a"),
									  (CFG.compress.create_new)?_T("-d1 -jso1 -+1 -jf0 -jtc -gm"):_T("-d1 -jso1 -+1 -jf0 -gm"),
									  //-d1    : ディレクトリー (配下) の格納。
									  //-jso1  : SH_DENYNO でのオープンを行わない。
									  //-+1    : レジストリーの設定を無視。
									  //-jf0   : ルート記号の削除。
									  //-jtc   : 指定したファイルのみ格納
									  //-gm    : エラーメッセージ表示の抑止。
									  CFG.general.custom_param.c_str(),

									  _T("-n1"),

									  //sfx
									  getMethod().cmd,

									  quotePath(arc_path_str).c_str(),
									  quotePath(list_file_path).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'に圧縮しています...\n\n"),arc_path);


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

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	unload();

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcUnlha32::ARC_RESULT ArcUnlha32::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	m_file_size=m_write_size=m_last_write_size=m_last_hash=0;

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
	tstring list_file_path;
	File list_file;

	if(use_filter){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("lzh"),ARCCFG->m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARC_CANNOT_OPEN_LISTFILE;
		}

		//リストファイルに解凍対象外ファイルを出力
		outputFileListEx(arc_path_str.c_str(),
						 CFG.general.filefilter,
						 CFG.general.file_ex_filter,
						 //フィルタ適用を逆にする
						 REVERSE_FILTER,
						 &list_file);
		list_file.close();
	}

	if(IS_TERMINATED)return ARC_FAILURE;

	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_file_size=getTotalOriginalSize(arc_path);
	}

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
	replaceDelimiter(list_file_path);

	tstring cmd_line(format(_T("%s %s %s %s %s %s"),
							(!CFG.general.ignore_directory_structures)?_T("e -x1"):_T("e"),
							_T("-a1 -c1 -jf0 -jyo -+1 -gm -r2"),
							//-a1    : すべてのファイルについて属性を復元して展開します。
							//-c1    : タイムスタンプ検査を省略
							//-jyo   : 既存ファイルへの上書き確認の省略。
							//-gm    : エラーメッセージ表示の抑止。
							//-r2    : ディレクトリー指定再帰モード
							CFG.general.custom_param.c_str(),

							_T("-n1"),

							quotePath(arc_path_str).c_str(),
							quotePath(output_dir_str).c_str()));

	if(use_filter){
		cmd_line.append(format(_T(" @%s"),
							   quotePath(list_file_path).c_str()));
	}

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'を解凍しています...\n\n"),arc_path);

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

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	//パス区切り文字を'\\'に
	if(*m_delimiter=='/')str::replaceCharacter(output_dir_str,'/','\\');

	if(CFG.general.decode_uesc){
		//ファイル名に含まれるUnicodeエスケープシーケンスをデコードする
		m_util->decodeUnicodeEscape(arc_path,output_dir_str.c_str(),CFG.general.ignore_directory_structures);
	}

	if(!CFG.general.ignore_directory_structures&&
	   CFG.extract.directory_timestamp){
		//ディレクトリの更新日時を復元
		m_util->recoverDirectoryTimestamp(arc_path,output_dir_str.c_str(),CFG.general.decode_uesc,true);
	}

	unload();

	if(CFG.compress.exclude_base_dir!=0){
		//共通パスを取り除く
		m_util->excludeCommonPath(output_dir_bak.c_str(),output_dir_str.c_str(),CFG.compress.exclude_base_dir);
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcUnlha32::ARC_RESULT ArcUnlha32::del(const TCHAR* arc_path_orig,tstring* log_msg){
	tstring arc_path(arc_path_orig);

	tstring list_file_path;
	File list_file;
	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	if(!use_filter||
	   !CFG.recompress.run_command.disable()||
	   CFG.general.ignore_directory_structures||
	   CFG.compress.compression_level!=default_compressionlevel){
		return ARC_FAILURE;
	}

	//リストファイルを作成
	list_file_path=tempfile::create(_T("lzh"),ARCCFG->m_list_temp_dir.c_str());
	if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
		return ARC_CANNOT_OPEN_LISTFILE;
	}

	if(!createFilesList(arc_path.c_str()))return ARC_FAILURE;

	std::vector<fileinfo::FILEINFO> fileinfo_list=m_arc_info.file_list;

	applyFilters(&fileinfo_list,CFG.general.filefilter,CFG.general.file_ex_filter,true);

	//リストファイルに解凍対象ファイルのみ出力
	for(std::vector<fileinfo::FILEINFO>::const_iterator ite=fileinfo_list.begin(),
		end=fileinfo_list.end();
		ite!=end;++ite){
		list_file.writeEx(_T("%s\r\n"),ite->name.c_str());
	}

	list_file.close();

	//区切り文字置換
	replaceDelimiter(arc_path);
	replaceDelimiter(list_file_path);

	tstring cmd_line(format(_T("%s %s %s %s @%s"),
									  _T("d"),
									  _T("-gm -r2"),
									  //-gm    : エラーメッセージ表示の抑止
									  //-r2    : ディレクトリー指定再帰モード

									  _T("-n1"),

									  quotePath(arc_path).c_str(),
									  quotePath(list_file_path).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'を処理しています...\n\n"),arc_path_orig);

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

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	unload();

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcUnlha32::ARC_RESULT ArcUnlha32::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	if(CFG.output_file_list.api_mode){
		outputFileListEx(arc_path_str.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter,(CFG.general.decode_uesc)?DECODE_UNICODE_ESCAPE:0);
	}else{
		tstring list_file_path;
		File list_file;

		bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

		if(use_filter){
			//リストファイルを作成
			list_file_path=tempfile::create(_T("lzh"),ARCCFG->m_list_temp_dir.c_str());
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

		replaceDelimiter(list_file_path);

		tstring cmd_line(format(_T("%s %s"),
								_T("l"),
								quotePath(arc_path_str).c_str()));

		if(use_filter){
			cmd_line.append(format(_T(" @%s"),
								   quotePath(list_file_path).c_str()));
		}

		dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

		tstring log_msg;

		execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	}
	return ARC_SUCCESS;
}

//リストファイルにファイルリストを出力
void ArcUnlha32::outputFileListToFile(const fileinfo::FILEINFO& fileinfo,int opt,File* list_file){
	if(list_file==NULL)return;
	tstring file_path(fileinfo.name);

	if(opt&DECODE_UNICODE_ESCAPE){
		//Unicodeエスケープをデコード
		str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
	}

	list_file->writeEx(_T("-jx%s\r\n"),quotePath(fileinfo.name).c_str());
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcUnlha32::writeFormatedPath(const File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig){
	tstring base_dir(base_dir_orig);
	tstring file_path(file_path_orig);

	//ディレクトリ名末尾の区切り文字は不要
	file_path=path::removeTailSlash(file_path);

	return list_file.writeEx(_T("%s %s\r\n"),
							 quotePath(base_dir+m_delimiter).c_str(),
							 quotePath(file_path).c_str());
}

//圧縮対象外ファイルのパスを整形してファイルに書き出す
DWORD ArcUnlha32::writeFormatedExcludePath(const File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig){
	tstring base_dir(base_dir_orig);
	tstring file_path(file_path_orig);
	tstring root_dir;

	//ディレクトリ名末尾の区切り文字は不要
	file_path=path::removeTailSlash(file_path);

	if(str::locateFirstCharacter(file_path.c_str(),*m_delimiter)!=-1){
		root_dir=path::getRootDirectory(file_path);
	}

	return list_file.writeEx(_T("%s %s -jx%s%s\r\n"),
							 quotePath(base_dir+m_delimiter).c_str(),

							 (!root_dir.empty())?quotePath(root_dir+m_delimiter+_T("*")).c_str():_T(""),

							 m_delimiter,
							 quotePath(file_path).c_str());
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcUnlha32::writeFormatedList(const File& list_file,const tstring& full_path){
	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	tstring base_dir=path::getParentDirectory(full_path);
	replaceDelimiter(base_dir);
	tstring file_name=path::getFileName(full_path);

	if(path::isDirectory(full_path.c_str())){
		//full_pathはディレクトリ

		if(CFG.compress.exclude_base_dir!=0){
			//基底ディレクトリを含まない
			base_dir=full_path;
			replaceDelimiter(base_dir);
		}

		std::list<size_info> relative_path_list;

		if(use_filter){
			//フィルタ使用時
			std::vector<fileinfo::FILEINFO> filtered_list;

			FileTree file_tree(CFG.general.filefilter,CFG.general.file_ex_filter);

			file_tree.createFileTree(full_path.c_str(),_T("*"),true);

			if(!CFG.general.ignore_directory_structures){
				//通常圧縮->処理対象外ファイルリスト
				file_tree.makeExcludeTree(FileTree::TO_NONE,full_path.c_str());
			}else{
				//階層無視圧縮->処理対象ファイルリスト出力(全てのディレクトリを含まない)
				file_tree.makeIncludeTree(FileTree::TO_EXCLUDE_DIR,full_path.c_str());
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
					relative_path_list.push_back(size_info(
						path::makeRelativePath(base_dir.c_str(),
											   FILE_ATTRIBUTE_DIRECTORY,
										   ite->name.c_str(),
											   ::GetFileAttributes(ite->name.c_str())),
						ite->size));
				}
			}
		}

		if(!use_filter){
			relative_path_list.push_back(size_info(
				path::makeRelativePath(base_dir.c_str(),
									   FILE_ATTRIBUTE_DIRECTORY,
									   full_path.c_str(),
									   ::GetFileAttributes(full_path.c_str())),
				fileoperation::getDirectorySize(full_path.c_str())));
		}

		if(!relative_path_list.empty()){
			//一つ以上追加された
			tstring root_dir(path::makeRelativePath(base_dir.c_str(),
													FILE_ATTRIBUTE_DIRECTORY,
													full_path.c_str(),
													::GetFileAttributes(full_path.c_str())));
			replaceDelimiter(root_dir);

			if(!CFG.general.ignore_directory_structures){
				//ディレクトリをまず書き出し
				if(CFG.compress.exclude_base_dir==0){
					writeFormatedPath(list_file,base_dir.c_str(),root_dir.c_str());
				}else{
					writeFormatedPath(list_file,base_dir.c_str(),_T("*"));
				}
			}

			if(!use_filter){
				//フィルタがなければここで終了
				if(!CFG.no_display.no_information&&
				   !STDOUT.isRedirected()){
					m_file_size+=relative_path_list.begin()->second;
				}
				return true;
			}

			long long exclude_file_size=0;

			for(std::list<size_info>::iterator ite=relative_path_list.begin(),
				end=relative_path_list.end();
				ite!=end;
				++ite){

				replaceDelimiter(ite->first);

				if(!CFG.general.ignore_directory_structures){
					//'/mc'
					//write:
					//[親ディレクトリ ディレクトリ内ファイルの相対パス]
					writeFormatedExcludePath(list_file,base_dir.c_str(),ite->first.c_str());

					//合計サイズ[処理対象外]
					exclude_file_size+=ite->second;
				}else{
					//'mC'
					if(str::countCharacter(ite->first.c_str(),*m_delimiter)){
						//相対パスにデリミタを含む場合

						//例:base_dir=foo
						//   ite->name.c_str()=bar/baz

						//相対パスからファイル名を除くパスの部分を取得
						//例:bar
						tstring tmp=path::getParentDirectory(ite->first.c_str());
						base_dir+=m_delimiter;
						//元の親ディレクトリのパスと取得したパスを結合
						//foo/bar
						base_dir+=tmp;
					}
					//ファイルの相対パスからファイル名を取得
					file_name=path::getFileName(ite->first);

					//write:
					//[ディレクトリパス] [区切り文字がないファイルパス]
					writeFormatedPath(list_file,base_dir.c_str(),file_name.c_str());

					//親のパスを元に戻す
					base_dir=path::getParentDirectory(full_path);
					replaceDelimiter(base_dir);
				}
				//合計サイズ
				if(!CFG.no_display.no_information&&
				   !STDOUT.isRedirected()){
					m_file_size+=ite->second;
				}
			}
			//合計サイズ
			if(!CFG.no_display.no_information&&
			   !STDOUT.isRedirected()){
				if(!CFG.general.ignore_directory_structures){
					m_file_size=fileoperation::getDirectorySize(full_path.c_str())-exclude_file_size;
				}
			}
		}
	}else{
		//full_pathはファイル
		if(fileinfo::matchFilters(full_path.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter,base_dir.c_str())){
			//合計サイズ
			if(!CFG.no_display.no_information&&
			   !STDOUT.isRedirected()){
				m_file_size+=fileoperation::getFileSize(full_path.c_str());
			}
			//write:
			//[親ディレクトリ ディレクトリ内ファイルの相対パス]
			writeFormatedPath(list_file,base_dir.c_str(),file_name.c_str());
		}
	}
	return true;
}

//ファイル処理情報を格納
void ArcUnlha32::setExtractingInfo(UINT state,void* arc_info){
	switch(m_extracting_info_struct_size){
		case sizeof(EXTRACTINGINFOEX64):{
			//処理中ファイル名
			//SourceとDest反転注意
			if(isUnicodeMode()){
				str::utf82utf16(&m_processing_info.file_name,((LPEXTRACTINGINFOEX64)arc_info)->szDestFileName);
			}else{
				str::sjis2utf16(&m_processing_info.file_name,((LPEXTRACTINGINFOEX64)arc_info)->szDestFileName);
			}

			//ファイルサイズ
			m_processing_info.total=m_file_size;

			//処理済みサイズ
			//ディレクトリをここで蹴る
			if(m_processing_info.file_name.rfind(m_delimiter)==m_processing_info.file_name.length()-1){
				return;
			}

			DWORD cur_hash=0;
			const TCHAR* key=m_processing_info.file_name.c_str();
			//重複しても書庫処理自体に影響はないので適当に...
			//	while(*key)cur_hash+=*(key++);
			cur_hash=0;
			while(*key)cur_hash=cur_hash*137+*(key++);
			cur_hash%=1987;

			long long write_size=0;

			if(((LPEXTRACTINGINFOEX64)arc_info)->llFileSize==0){
				m_processing_info.done=m_write_size;
				break;
			}

			if(m_last_hash!=0&&
			   m_last_hash==cur_hash){
				//Unlha32は圧縮時にまず一時ファイルの情報??を送ってきてくれる優しい子ので
				//格納ファイル一つ一つの書き込み状況に就いては更新せず、
				//格納ファイルのサイズを取り扱うように。
				if(CFG.mode==MODE_COMPRESS)break;
				write_size=abs(m_last_write_size-((LPEXTRACTINGINFOEX64)arc_info)->llWriteSize);
			}else{
				write_size=((LPEXTRACTINGINFOEX64)arc_info)->llWriteSize;
			}

			m_last_write_size=((LPEXTRACTINGINFOEX64)arc_info)->llWriteSize;

			if(CFG.mode==MODE_COMPRESS)write_size=((LPEXTRACTINGINFOEX64)arc_info)->llFileSize;

			m_processing_info.done=
				m_write_size+=write_size;
			if(cur_hash!=0)m_last_hash=cur_hash;
			break;
		}
		case sizeof(EXTRACTINGINFOEX):
			break;
	}
}
