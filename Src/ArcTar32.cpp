//ArcTar32.cpp
//Tar32.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r23 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcTar32.h"
#include"ArcCfg.h"
#include"FileInfo.h"

using namespace sslib;




ArcTar32::ArcTar32():
	ArcDll(_T("Tar32"),
			_T("Tar"),
			_T("tar.tgz.gz.bz2.tbz.tbz2.xz.txz.lzma.tlz.tlzma.Z.taz.tz.cpio.cpgz.a.lib.rpm.deb"),
			_T("\\")),
	m_file_size(0),
	m_write_size(0),
	m_last_write_size(0){
		COMPRESSION_METHOD method[]={
			//-Gはその圧縮単体(tarで固めない)
			{_T("tar"),_T(".tar"),_T(""),_T(""),
				0,
				-1,-1,-1},

			{_T("gz"),_T(".gz"),_T("-Gz"),_T("-Gz"),
				0,
				6,1,9},
			{_T("gzip"),_T(".gz"),_T("-Gz"),_T("-Gz"),
				0,
				6,1,9},
			{_T("tgz"),_T(".tgz"),_T("-z"),_T("-z"),
				0,
				6,1,9},
			{_T("targzip"),_T(".tar.gz"),_T("-z"),_T("-z"),
				0,
				6,1,9},

			{_T("bz2"),_T(".bz2"),_T("-GB"),_T("-GB"),
				0,
				9,1,9},
			{_T("bzip"),_T(".bz2"),_T("-GB"),_T("-GB"),
				0,
				9,1,9},
			{_T("bzip2"),_T(".bz2"),_T("-GB"),_T("-GB"),
				0,
				9,1,9},
			{_T("tbz"),_T(".tbz"),_T("-B"),_T("-B"),
				0,
				9,1,9},
			{_T("tarbzip"),_T(".tar.bz2"),_T("-B"),_T("-B"),
				0,
				9,1,9},
			{_T("tarbz2"),_T(".tar.bz2"),_T("-B"),_T("-B"),
				0,
				9,1,9},
			{_T("tarbzip2"),_T(".tar.bz2"),_T("-B"),_T("-B"),
				0,
				9,1,9},

			{_T("xz"),_T(".xz"),_T("-GJ"),_T("-GJ"),
				0,
				6,0,9},
			{_T("tarxz"),_T(".tar.xz"),_T("-J"),_T("-J"),
				0,
				6,0,9},
			{_T("txz"),_T(".txz"),_T("-J"),_T("-J"),
				0,
				6,0,9},

			{_T("lzma"),_T(".lzma"),_T("-G--lzma"),_T("-G--lzma="),
				0,
				6,0,9},
			{_T("tarlzma"),_T(".tar.lzma"),_T("--lzma"),_T("--lzma="),
				0,
				6,0,9},
			{_T("tlz"),_T(".tlz"),_T("--lzma"),_T("--lzma="),
				0,
				6,0,9},
			{_T("tlzma"),_T(".tlzma"),_T("--lzma"),_T("--lzma="),
				0,
				6,0,9},
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_methods.assign(method,method+ARRAY_SIZEOF(method));
}

//圧縮形式を取得(その形式に対応している場合のみ)
tstring ArcTar32::getCompressionMethod(const TCHAR* arc_path_orig){
	tstring arc_path(arc_path_orig);
	replaceDelimiter(arc_path);

	switch(getArchiveType(arc_path.c_str())){
		case ARCHIVETYPE_TAR:
			return _T("tar");
			break;
		case ARCHIVETYPE_TARGZ:
			return _T("targzip");
			break;
		case ARCHIVETYPE_TARZ:
			break;
		case ARCHIVETYPE_GZ:
			return _T("gz");
			break;
		case ARCHIVETYPE_Z:
			break;
		case ARCHIVETYPE_TARBZ2:
			return _T("tarbzip2");
			break;
		case ARCHIVETYPE_BZ2:
			return _T("bz2");
			break;
		case ARCHIVETYPE_TARLZMA:
			return _T("tarlzma");
			break;
		case ARCHIVETYPE_TARXZ:
			return _T("tarxz");
			break;
		case ARCHIVETYPE_LZMA:
			return _T("lzma");
			break;
		case ARCHIVETYPE_XZ:
			return _T("xz");
			break;
		default:
		case ARCHIVETYPE_NORMAL:
		case ARCHIVETYPE_CPIO:
		case ARCHIVETYPE_CPIOGZ:
		case ARCHIVETYPE_CPIOZ:
		case ARCHIVETYPE_CPIOBZ2:
		case ARCHIVETYPE_CPIOLZMA:
		case ARCHIVETYPE_CPIOXZ:
		case ARCHIVETYPE_AR:
		case ARCHIVETYPE_ARGZ:
		case ARCHIVETYPE_ARZ:
		case ARCHIVETYPE_ARBZ2:
		case ARCHIVETYPE_ARLZMA:
		case ARCHIVETYPE_ARXZ:
			break;
	}
	return _T("");
}

ArcTar32::ARC_RESULT ArcTar32::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	if(getMethod().mhd==NULL)return ARC_FAILURE;

	tstring arc_path_str(arc_path);

	m_file_size=m_write_size=m_last_write_size=0;

	//階層無視圧縮ではリスト出力が必要!
	if(CFG.general.ignore_directory_structures&&
	   (CFG.general.filefilter.empty()&&CFG.general.file_ex_filter.empty())){
		CFG.general.filefilter.pattern_list.push_back(tstring(_T("*")));
	}

	//書庫にファイルを追加する
	bool add_files=!CFG.compress.create_new&&
					path::fileExists(arc_path);

	tstring list_file_path;
	File list_file;

	//リストファイルを作成
	list_file_path=fileoperation::createTempFile(_T("tar"),ARCCFG->m_list_temp_dir.c_str());
	if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
		return ARC_CANNOT_OPEN_LISTFILE;
	}

	if(add_files){
		//tar32.dllには現在書庫追加コマンドが無い為、
		//書庫を一時ディレクトリに解凍し、
		//リストファイルにて追加ファイルと併せて指定するように
		bool no_information=CFG.no_display.no_information;

		CFG.no_display.no_information=true;
		m_disable_callback=true;

		tstring log;

		if(extract(arc_path,ARCCFG->m_recmp_temp_dir.c_str(),&log)!=ARC_SUCCESS){
			CFG.no_display.no_information=no_information;
			return ARC_FAILURE;
		}
		m_disable_callback=false;
		CFG.no_display.no_information=no_information;

		::SetCurrentDirectory(ARCCFG->m_recmp_temp_dir.c_str());
	}

	//リストファイルにパスを出力
	if(CFG.mode==MODE_RECOMPRESS||add_files){
		list_file.writeEx(_T("*\r\n"));

		//ファイルサイズ計算
		if(!CFG.no_display.no_information&&
		   !STDOUT.isRedirected()){
			m_file_size=fileoperation::getDirectorySize(path::getCurrentDirectory().c_str());
		}
	}

	if(CFG.mode==MODE_COMPRESS){
		for(std::list<tstring>::const_iterator ite=file_list->begin(),
			end=file_list->end();
			ite!=end;++ite){
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
	arc_path_str+=_T(".");

	//圧縮率の処理
	tstring level_str;

	if(CFG.compress.compression_level==minimum_compressionlevel){
		//'/l'と指定された場合
		CFG.compress.compression_level=getMethod().minimum_level;
	}else if(CFG.compress.compression_level==maximum_compressionlevel){
		//'/lx'と指定された場合
		CFG.compress.compression_level=getMethod().maximum_level;
	}

	if(CFG.compress.compression_level!=-1){
		//圧縮率を範囲内に収める
		CFG.compress.compression_level=clamp(CFG.compress.compression_level,
											   getMethod().minimum_level,
											   getMethod().maximum_level);
		level_str=getMethod().level;
		level_str+=static_cast<TCHAR>(CFG.compress.compression_level+'0');
	}

	VariableArgument cmd_line(_T("%s %s %s %s %s %s %s%s%s @%s%s%s"),
							  _T("-c"),
							  _T("--display-dialog=0"),
							  _T("--inverse-procresult=1"),
							  //--inverse-procresult=1   : ARCHIVERPROCの返し値を反転します。
							  //getMethod().level,compression_level
							  (CFG.compress.compression_level!=-1&&!level_str.empty())?level_str.c_str():getMethod().cmd,
							  CFG.general.custom_param.c_str(),

							  _T("--"),

							  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),
							  arc_path_str.c_str(),
							  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),

							  (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
							  list_file_path.c_str(),
							  (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'を処理しています...\n\n"),arc_path);


	//実行
	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	if(CFG.no_display.no_log){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.get(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
	}

	unload();

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcTar32::ARC_RESULT ArcTar32::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	m_file_size=m_write_size=m_last_write_size=0;

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
	tstring list_file_path;
	File list_file;

	if(use_filter){
		//リストファイルを作成
		list_file_path=fileoperation::createTempFile(_T("tar"),ARCCFG->m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
			return ARC_CANNOT_OPEN_LISTFILE;
		}

		//リストファイルに解凍対象ファイルのみ出力
		if(!outputFileListEx(arc_path_str.c_str(),
							 CFG.general.filefilter,
							 CFG.general.file_ex_filter,
							 list_file)){
			return ARC_NO_MATCHES_FOUND;
		}
		list_file.close();
	}

	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_file_size=getTotalOriginalSize(arc_path);
	}

	tstring output_dir_bak;
	fileoperation::scheduleDelete schedule_delete;

	if(CFG.compress.exclude_base_dir!=0){
		output_dir_bak=output_dir_str;
		output_dir_str=path::addTailSlash(fileoperation::createTempDir(_T("rcs"),output_dir_str.c_str()));

		//一時ディレクトリ削除予約
		schedule_delete.set(output_dir_str.c_str());
		//削除漏れ対策
		ARCCFG->m_schedule_list.push_back(new fileoperation::scheduleDelete(output_dir_str.c_str()));
	}

	//区切り文字置換
	replaceDelimiter(arc_path_str);
	replaceDelimiter(output_dir_str);

	VariableArgument cmd_line(_T("%s %s %s %s %s %s%s%s %s%s%s"),
							  (!CFG.general.ignore_directory_structures)?_T("-x"):_T("--use-directory=0 -x"),
							  //--use-directory=0        : effective directory name
							  _T("--display-dialog=0"),
							  _T("--inverse-procresult=1"),
							  //--inverse-procresult=1   : ARCHIVERPROCの返し値を反転します。
							  CFG.general.custom_param.c_str(),

							  _T("--"),

							  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),
							  arc_path_str.c_str(),
							  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),

							  (str::containsWhiteSpace(output_dir_str))?_T("\""):_T(""),
							  output_dir_str.c_str(),
							  (str::containsWhiteSpace(output_dir_str))?_T("\""):_T(""));

	if(use_filter){
		cmd_line.add(_T(" @%s%s%s"),
					 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""),
					 list_file_path.c_str(),
					 (str::containsWhiteSpace(list_file_path))?_T("\""):_T(""));
	}

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'を処理しています...\n\n"),arc_path);

	//実行
	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	if(CFG.no_display.no_log){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.get(),&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
	}

	//パス区切り文字を'\\'に
	if(*m_delimiter=='/')str::replaceCharacter(output_dir_str,'/','\\');

	if(CFG.general.decode_uesc){
		//ファイル名に含まれるUnicodeエスケープシーケンスをデコードする
		decodeUnicodeEscape(arc_path,output_dir_str.c_str(),CFG.general.ignore_directory_structures);
	}

	if(!CFG.general.ignore_directory_structures&&
	   CFG.extract.directory_timestamp){
		//ディレクトリの更新日時を復元
		recoverDirectoryTimestamp(arc_path,output_dir_str.c_str(),CFG.general.decode_uesc,true);
	}

	unload();

	if(CFG.compress.exclude_base_dir!=0){
		//共通パスを取り除く
		excludeCommonPath(output_dir_bak.c_str(),output_dir_str.c_str(),CFG.compress.exclude_base_dir);
	}

	return (dll_ret==0)?ARC_SUCCESS:ARC_FAILURE;
}

ArcTar32::ARC_RESULT ArcTar32::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	if(CFG.output_file_list.api_mode){
		outputFileListEx(arc_path_str.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter,(CFG.general.decode_uesc)?DECODE_UNICODE_ESCAPE:0);
	}else{
		VariableArgument cmd_line(_T("%s %s %s %s%s%s"),
								  _T("-l"),
								  _T("--display-dialog=0"),
								  _T("--"),
								  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""),
								  arc_path_str.c_str(),
								  (str::containsWhiteSpace(arc_path_str))?_T("\""):_T(""));

		dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.get());

		tstring log_msg;

		execute(NULL,cmd_line.get(),&log_msg,log_buffer_size);
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	}
	return ARC_SUCCESS;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcTar32::writeFormatedPath(const File& list_file,const TCHAR* base_dir_orig,const TCHAR* file_path_orig){
	tstring base_dir(base_dir_orig);
	tstring file_path(file_path_orig);

	//ディレクトリ名末尾の区切り文字は不要
	file_path=path::removeTailSlash(file_path);

	return list_file.writeEx(_T("%s%s%s%s %s%s%s\r\n"),
							 (str::containsWhiteSpace(base_dir))?_T("\""):_T(""),
							 base_dir.c_str(),
							 m_delimiter,
							 (str::containsWhiteSpace(base_dir))?_T("\""):_T(""),
							 (str::containsWhiteSpace(file_path))?_T("\""):_T(""),
							 file_path.c_str(),
							 (str::containsWhiteSpace(file_path))?_T("\""):_T(""));
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcTar32::writeFormatedList(const File& list_file,const tstring& full_path){
	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();

	tstring base_dir(path::getParentDirectory(full_path));
	//unlha32.dll以外は不要なはず...
//	replaceDelimiter(base_dir);
	tstring file_name(path::getFileName(full_path));

	if(path::isDirectory(full_path.c_str())&&
	   !path::isEmptyDirectory(full_path.c_str())){
		//full_pathはディレクトリ

		if(CFG.compress.exclude_base_dir!=0){
			//基底ディレクトリを含まない
			base_dir=full_path;
//			replaceDelimiter(base_dir);
		}

		std::list<size_info> relative_path_list;

		if(use_filter){
			//フィルタ使用時
			std::vector<fileinfo::FILEINFO> exclude_list;

			FileTree file_tree(CFG.general.filefilter,CFG.general.file_ex_filter);
			file_tree.createFileTree(full_path.c_str());
			file_tree.makeIncludeTree(FileTree::TO_EXCLUDE_DIR,full_path.c_str());

			//リストに変換
			file_tree.tree2list(exclude_list);

			if(exclude_list.empty()){
				//フィルタを使用しない処理へ
				use_filter=false;
			}else{
				for(std::vector<fileinfo::FILEINFO>::iterator ite=exclude_list.begin(),
					end=exclude_list.end();
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

			if(CFG.compress.exclude_base_dir!=0&&!use_filter){
				//'eb'が有効かつフィルタなし
				writeFormatedPath(list_file,base_dir.c_str(),_T("*"));
			}

			for(std::list<size_info>::iterator ite=relative_path_list.begin(),
				end=relative_path_list.end();
				ite!=end;
				++ite){
				if(!CFG.general.ignore_directory_structures){
					//'/mc'
					//write:
					//[親ディレクトリ ディレクトリ内ファイルの相対パス]
					writeFormatedPath(list_file,base_dir.c_str(),ite->first.c_str());
				}else{
					//'mC'
					if(str::countCharacter(ite->first.c_str(),*m_delimiter)){
						//相対パスにデリミタを含む場合

						//例:base_dir=foo
						//   ite->name.c_str()=bar/baz

						//相対パスからファイル名を除くパスの部分を取得
						//例:bar
						tstring tmp=path::getParentDirectory(ite->first);
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
void ArcTar32::setExtractingInfo(UINT state,void* arc_info){
	//dwFileSizeは処理中のファイルのサイズ
	//dwWriteSizeは処理中ファイルの書き込まれたサイズ
	switch(m_extracting_info_struct_size){
		case sizeof(EXTRACTINGINFOEX64):
			break;

		case sizeof(EXTRACTINGINFOEX):{
			//処理中ファイル名
			//SourceとDest反転注意
			if(isUnicodeMode()){
				str::utf82utf16(&m_processing_info.file_name,((LPEXTRACTINGINFOEX)arc_info)->exinfo.szDestFileName);
			}else{
				str::sjis2utf16(&m_processing_info.file_name,((LPEXTRACTINGINFOEX)arc_info)->exinfo.szDestFileName);
			}

			//ファイルサイズ
			m_processing_info.total=m_file_size;

			//処理済みサイズ
			//ディレクトリをここで蹴る
			if(m_processing_info.file_name.rfind(m_delimiter)==m_processing_info.file_name.length()-1){
				return;
			}

			DWORD write_size=0;

			if(((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwFileSize==0){
				m_processing_info.done=m_write_size;
				break;
			}

			if(((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize==0){
				//初めてそのファイルの処理を行う場合
				write_size=((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize;
			}else{
				//前回から継続して同じファイルの処理する場合
				write_size=(m_last_write_size>((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize)?
					m_last_write_size-((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize:
					((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize-m_last_write_size;
			}

			m_last_write_size=((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize;

			m_processing_info.done=
				m_write_size+=write_size;
			break;
		}
	}
}
