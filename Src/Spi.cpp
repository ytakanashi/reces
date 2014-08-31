//Spi.cpp
//Spi操作クラス
//一部の関数のみに対応(書庫関連)

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r22 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"Spi.h"

#include<iterator>

using namespace sslib;


//spiである[戻り値はプラグインのタイプ]
int Spi::isSusiePlugin(){
	if(!isLoaded())load();

	tstring type(12,'\0');

	if(getPluginInfo(SPI_GET_VERSION,&type,12)==0){
		return SPI_NOT;
	}else{
		return (type.substr(type.length()-2)==_T("IN"))?SPI_IN:SPI_AM;
	}
}

//対応している書庫か
bool Spi::isSupportedArchive(const TCHAR* arc_path){
	if(!isLoaded())load();

	tstring ext=_T("*.")+str::toLower(path::getExtension(arc_path));
	tstring supported_ext;
	std::list<tstring> supported_ext_list;

	for(size_t i=0;;++i){
		tstring tmp(256,'\0');

		getPluginInfo(2*i+SPI_GET_EXT,&tmp,tmp.length());
		if(tmp.empty())break;
		supported_ext+=tmp;
		tmp.clear();
	}

	str::splitString(&supported_ext_list,str::toLower(supported_ext).c_str(),';');
	supported_ext_list.sort();
	supported_ext_list.unique();

	for(std::list<tstring>::iterator ite=supported_ext_list.begin(),
		end=supported_ext_list.end();
		ite!=end;
		++ite){
		if(str::matchWildcards(ext.c_str(),ite->c_str())){
			return true;
		}
	}
	return false;
}

//プラグインに就いての情報を取得
tstring Spi::getAboutStr(){
	if(!isLoaded())load();

	tstring about(256,'\0');

	getPluginInfo(SPI_GET_ABOUT,&about,about.length());

	return about;
}

//設定ダイアログを表示
bool Spi::configDialog(HWND wnd_handle){
	return ConfigurationDlg(wnd_handle)==0;
}

int Spi::getFileCount(const TCHAR* arc_path){
	if(!m_arc_info.empty())return m_arc_info.size();

	if(!isLoaded())load();

	int count=0;
	HLOCAL h_info=NULL;

	if(getArchiveInfo(arc_path,0,SPI_INPUT_FILE,&h_info)!=SPI_ERROR_SUCCESS){
		return -1;
	}

	Spi::fileInfo* arc_info=reinterpret_cast<fileInfo*>(::LocalLock(h_info));

	if(!arc_info){
		return -1;
	}

	for(;arc_info->method[0];++arc_info){
		count++;
	}
	::LocalUnlock(h_info);
	::LocalFree(h_info);
	return count;
}

//エラーメッセージを取得
bool Spi::getErrorMessage(tstring* msg,int code){
	if(msg==NULL)return false;

	switch(code){
		case SPI_ERROR_NOT_IMPLEMENT:
			msg->assign(_T("その機能はインプリメントされていない"));
			break;
		case SPI_ERROR_SUCCESS:
			msg->assign(_T("正常終了"));
			break;
		case SPI_ERROR_USER_CANCEL:
			msg->assign(_T("コールバック関数が非0を返したので展開を中止した"));
			break;
		case SPI_ERROR_UNKNOWN_FORMAT:
			msg->assign(_T("未知のフォーマット"));
			break;
		case SPI_ERROR_BROKEN_DATA:
			msg->assign(_T("データが壊れている"));
			break;
		case SPI_ERROR_MALLOC:
			msg->assign(_T("メモリーが確保出来ない"));
			break;
		case SPI_ERROR_MEMORY:
			msg->assign(_T("メモリーエラー（Lock出来ない、等）"));
			break;
		case SPI_ERROR_FILE_READ:
			msg->assign(_T("ファイルリードエラー"));
			break;
		case SPI_ERROR_WINDOW:
			msg->assign(_T("窓が開けない （非公開）"));
			break;
		case SPI_ERROR_INTERNAL:
			msg->assign(_T("内部エラー"));
			break;
		case SPI_ERROR_FILE_WRITE:
			msg->assign(_T("ファイル書き込みエラー (非公開)"));
			break;
		case SPI_ERROR_EOF:
			msg->assign(_T("ファイル終端を検出 (非公開)"));
			break;
		default:
			break;
	}
	return true;
}

bool Spi::extract(const TCHAR* arc_path,const TCHAR* output_dir_orig,unsigned int progress_thread_id,tstring* log_msg){
	if(!isLoaded())load();

	bool result=true;
	tstring output_dir(path::addTailSlash(output_dir_orig));

	HLOCAL h_info=NULL;

	if(getArchiveInfo(arc_path,0,SPI_INPUT_FILE,&h_info)!=SPI_ERROR_SUCCESS){
		return false;
	}

	fileInfo* arc_info=reinterpret_cast<fileInfo*>(::LocalLock(h_info));
	fileInfo* arc_info_header=arc_info;

	if(!arc_info){
		return false;
	}

	int file_count=getFileCount(arc_path);

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

	if(log_msg!=NULL&&
	   !m_arc_cfg.cfg().no_display.no_log){
		log_msg->append(_T("\n"));
	}

	for(;arc_info->method[0]&&!isTerminated()&&!m_arc_cfg.m_password_input_cancelled;++arc_info){
		if(!matchFilters(fileinfo::FILEINFO(str::sjis2utf16(arc_info->path)+str::sjis2utf16(arc_info->filename),
											arc_info->filesize,
											0,
											((long long)arc_info->timestamp+11644473600)*10000000),
						 m_arc_cfg.cfg().general.filefilter,
						 m_arc_cfg.cfg().general.file_ex_filter))continue;

		tstring file_name=str::sjis2utf16(arc_info->filename);

		if(!m_arc_cfg.cfg().general.ignore_directory_structures){
			if(file_name.rfind(_T("/"))==file_name.length()-1||
			   file_name.rfind(_T("\\"))==file_name.length()-1){

				//ディレクトリ名に含まれるUnicodeエスケープシーケンスをデコードする
				tstring decoded_path(output_dir+(str::sjis2utf16(arc_info->path)+file_name));

				if(m_arc_cfg.cfg().general.decode_uesc){
					str::decodeUnicodeEscape(decoded_path,decoded_path.c_str(),false,'#');
				}

				fileoperation::createDirectory(decoded_path.c_str());
				continue;
			}else{
				//相対パスがpathの他にfilenameに代入されるものもあるため
				tstring dir_name=str::sjis2utf16(arc_info->path)+str::sjis2utf16(arc_info->filename);
				tstring parent_dir=path::getParentDirectory(dir_name);

				if(dir_name!=parent_dir){
					//ディレクトリ名に含まれるUnicodeエスケープシーケンスをデコードする
					tstring decoded_path(output_dir+parent_dir);

					if(m_arc_cfg.cfg().general.decode_uesc){
						str::decodeUnicodeEscape(decoded_path,decoded_path.c_str(),false,'#');
					}

					fileoperation::createDirectory(decoded_path.c_str());
				}
			}
		}

		if(progress_thread_id){
			ARC_PROCESSING_INFO spi_processing_info((arc_info-arc_info_header)+1,file_count,str::sjis2utf16(arc_info->path)+file_name);

			misc::thread::post(progress_thread_id,WM_UPDATE_PROGRESSBAR,reinterpret_cast<void*>(&spi_processing_info));
		}

		HLOCAL file;
		File dest;
		tstring err_msg;

		if(!m_arc_cfg.cfg().general.ignore_directory_structures){
			dest.open((output_dir+(str::sjis2utf16(arc_info->path)+file_name)).c_str(),OPEN_ALWAYS);
		}else{
			dest.open((output_dir+file_name).c_str(),OPEN_ALWAYS);
		}

		if(!dest.isOpened())continue;


		{
			int ret_code=getFile(arc_path,arc_info->position,&file,SPI_INPUT_FILE|SPI_OUTPUT_MEMORY,NULL,0);

			if(log_msg!=NULL&&
			   !m_arc_cfg.cfg().no_display.no_log){
				if(getErrorMessage(&err_msg,ret_code)){
					log_msg->append(err_msg+_T("   ")+file_name+_T("\n"));
				}
			}
			result=SPI_ERROR_SUCCESS==ret_code;
		}

		const void* data=::LocalLock(file);

		dest.write(data,arc_info->filesize);

		if(arc_info->timestamp){
			FILETIME ft={};

			ft=str::lltoft(((long long)arc_info->timestamp+11644473600)*10000000);

			dest.setFileTime(&ft);
		}

		tstring cur_path(dest.filepath());

		dest.close();

		//ファイル名に含まれるUnicodeエスケープシーケンスをデコードする
		if(m_arc_cfg.cfg().general.decode_uesc){
			tstring decoded_path;

			str::decodeUnicodeEscape(decoded_path,cur_path.c_str(),false,'#');

			fileoperation::renameFile(cur_path.c_str(),decoded_path.c_str());
		}

		::LocalUnlock(file);
		::LocalFree(file);
	}

	std::vector<tstring> recovered_dirs;
	std::vector<tstring> incomplete_dirs;

	//ディレクトリのタイムスタンプを復元
	if(!m_arc_cfg.cfg().general.ignore_directory_structures&&
	   m_arc_cfg.cfg().extract.directory_timestamp){
		for(arc_info=arc_info_header;arc_info->method[0]&&!isTerminated();++arc_info){
			tstring file_name=str::sjis2utf16(arc_info->filename);

			if(file_name.rfind(_T("/"))==file_name.length()-1||
			   file_name.rfind(_T("\\"))==file_name.length()-1){
				if(arc_info->timestamp){
					FILETIME ft={};

					ft=str::lltoft(((long long)arc_info->timestamp+11644473600)*10000000);

					tstring cur_path(output_dir+
									  (str::sjis2utf16(arc_info->path)+
									   file_name));
					tstring decoded_path(cur_path);

					//Unicodeエスケープがあれば変換
					str::decodeUnicodeEscape(decoded_path,cur_path.c_str(),false,'#');

					File dest(decoded_path.c_str());

					dest.setFileTime(&ft);

					recovered_dirs.push_back(decoded_path);
				}
			}else{
				DirTimeStamp::addIncompleteDir(output_dir,str::sjis2utf16(arc_info->path)+file_name,&incomplete_dirs);
			}
		}
		{
			std::vector<tstring> v;

			if(!incomplete_dirs.empty()&&
			   !recovered_dirs.empty()){
				std::sort(incomplete_dirs.begin(),incomplete_dirs.end());
				std::sort(recovered_dirs.begin(),recovered_dirs.end());
				std::set_difference(incomplete_dirs.begin(),
									incomplete_dirs.end(),
									recovered_dirs.begin(),
									recovered_dirs.end(),
									std::inserter(v,v.end()));
				incomplete_dirs=v;
			}
		}

		FILETIME arc_ft={};

		{
			File arc(arc_path);

			arc.getFileTime(&arc_ft);
		}

		for(int i=(!incomplete_dirs.empty())?incomplete_dirs.size()-1:-1;i>=0;--i){
				//配下のファイルやディレクトリを利用してディレクトリのタイムスタンプを復元
				DirTimeStamp::recover(incomplete_dirs[i].c_str(),arc_ft,recovered_dirs);
		}
	}

	if(m_arc_cfg.cfg().compress.exclude_base_dir!=0){
		//共通パスを取り除く
		excludeCommonPath(output_dir_bak.c_str(),output_dir.c_str(),m_arc_cfg.cfg().compress.exclude_base_dir);
	}

	::LocalUnlock(h_info);
	::LocalFree(h_info);

	m_arc_info.clear();
	return result;
}

bool Spi::list(const TCHAR* arc_path){
	if(!isLoaded())load();

	HLOCAL h_info=NULL;

	if(getArchiveInfo(arc_path,0,SPI_INPUT_FILE,&h_info)!=SPI_ERROR_SUCCESS){
		return false;
	}

	Spi::fileInfo* arc_info=reinterpret_cast<fileInfo*>(::LocalLock(h_info));

	if(!arc_info){
		return false;
	}

	if(!m_arc_cfg.cfg().output_file_list.api_mode){
		app()->stdOut().outputString(_T("\n   Date      Time         Size Name\n"));
		app()->stdOut().outputString(_T("---------- -------- ---------- ------------------------\n"));
	}

	for(;arc_info->method[0]&&!isTerminated();++arc_info){
		tstring file_name=str::sjis2utf16(arc_info->filename);

		if(!matchFilters(fileinfo::FILEINFO(str::sjis2utf16(arc_info->path)+file_name,
											arc_info->filesize,
											(file_name.rfind(_T("/"))==file_name.length()-1||file_name.rfind(_T("\\"))==file_name.length()-1)?FILE_ATTRIBUTE_DIRECTORY:0,
											((long long)arc_info->timestamp+11644473600)*10000000),
						 m_arc_cfg.cfg().general.filefilter,
						 m_arc_cfg.cfg().general.file_ex_filter))continue;

		if(!m_arc_cfg.cfg().output_file_list.api_mode){
			if(arc_info->timestamp){
				FILETIME ft={},tmp={};
				SYSTEMTIME st;

				tmp=str::lltoft(((long long)arc_info->timestamp+11644473600)*10000000);

				if(!::FileTimeToLocalFileTime(&tmp,&ft)){
					continue;
				}
				if(!::FileTimeToSystemTime(&ft,&st)){
					continue;
				}

				app()->stdOut().outputString(_T("%04u/%02u/%02u %02u:%02u:%02u "),
									  st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			}else{
				//タイムスタンプが存在しない
				app()->stdOut().outputString(_T("                    "));
			}

			tstring file_path(str::sjis2utf16(arc_info->path)+file_name);

			if(m_arc_cfg.cfg().general.decode_uesc){
				//Unicodeエスケープをデコード
				str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
			}

			app()->stdOut().outputString(_T("%10d %s\n"),
								  arc_info->filesize,
								  file_path.c_str());
		}else{
			tstring file_path(str::sjis2utf16(arc_info->path)+file_name);

			if(m_arc_cfg.cfg().general.decode_uesc){
				//Unicodeエスケープをデコード
				str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
			}

			if(file_name.rfind(_T("/"))==file_name.length()-1||
			   file_name.rfind(_T("\\"))==file_name.length()-1){
				//ディレクトリ
				app()->stdOut().outputString(Console::LOW_YELLOW,Console::NONE,_T("%s\n"),
									  file_path.c_str());
			}else{
				app()->stdOut().outputString(_T("%s\n"),
									  file_path.c_str());
			}
		}
	}
	::LocalUnlock(h_info);
	::LocalFree(h_info);

	m_arc_info.clear();
	return true;
}

//書庫内のすべてのファイルの情報を取得
bool Spi::createFilesList(const TCHAR* arc_path){
	//作成済み
	if(!m_arc_info.empty())return true;

	if(!isLoaded())load();

	HLOCAL h_info=NULL;

	if(getArchiveInfo(arc_path,0,SPI_INPUT_FILE,&h_info)!=SPI_ERROR_SUCCESS){
		return false;
	}

	Spi::fileInfo* arc_info=reinterpret_cast<fileInfo*>(::LocalLock(h_info));

	if(!arc_info){
		return false;
	}

	for(;arc_info->method[0];++arc_info){
		tstring file_name=str::sjis2utf16(arc_info->filename);

		m_arc_info.push_back(fileinfo::FILEINFO(str::sjis2utf16(arc_info->path)+file_name,
													 arc_info->filesize,
													 (file_name.rfind(_T("/"))==file_name.length()-1||file_name.rfind(_T("\\"))==file_name.length()-1)?FILE_ATTRIBUTE_DIRECTORY:0,
													 ((long long)arc_info->timestamp+11644473600)*10000000));
	}
	::LocalUnlock(h_info);
	::LocalFree(h_info);
	return !m_arc_info.empty();
}

//作成しようとするディレクトリは不要であるかどうか
bool Spi::isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file){
	if(!check_double_dir&&!check_only_file)return false;

	bool result=false;
	int files_count=0;

	RedundantDir redundant_dir;

	if(!m_arc_info.empty()){
		for(std::list<fileinfo::FILEINFO>::iterator ite=m_arc_info.begin(),
			end=m_arc_info.end();
			ite!=end;
			++ite){
			if(redundant_dir(*ite)){
				break;
			}else{
				continue;
			}
		}
		files_count=m_arc_info.size();
	}else{
		if(!isLoaded())load();

		HLOCAL h_info=NULL;

		if(getArchiveInfo(arc_path,0,SPI_INPUT_FILE,&h_info)!=SPI_ERROR_SUCCESS){
			return false;
		}

		Spi::fileInfo* arc_info=reinterpret_cast<fileInfo*>(::LocalLock(h_info));

		if(!arc_info){
			return false;
		}

		bool check_end=false;

		for(;arc_info->method[0];++arc_info){
			tstring file_name=str::sjis2utf16(arc_info->filename);

			m_arc_info.push_back(fileinfo::FILEINFO(str::sjis2utf16(arc_info->path)+file_name,
														 arc_info->filesize,
														 (file_name.rfind(_T("/"))==file_name.length()-1||file_name.rfind(_T("\\"))==file_name.length()-1)?FILE_ATTRIBUTE_DIRECTORY:0,
														 ((long long)arc_info->timestamp+11644473600)*10000000));
			if(!check_end){
				if(redundant_dir(m_arc_info.back())){
					files_count=m_arc_info.size();
					//リストを作成しておく為ループ継続
					check_end=true;
				}else{
					continue;
				}
			}
		}
		::LocalUnlock(h_info);
		::LocalFree(h_info);

		if(!check_end){
			files_count=m_arc_info.size();
		}
	}

	bool is_double_dir=redundant_dir.isDoubleDir();
	bool has_dir=redundant_dir.hasDir();

	if(is_double_dir&&!has_dir){
		is_double_dir=false;
	}

	if(check_double_dir&&is_double_dir){
		//二重ディレクトリ
		result=true;
	}else if(check_only_file&&
			 files_count==1&&
			 !(m_arc_info.begin()->attr&FILE_ATTRIBUTE_DIRECTORY)){
		//ファイル単体のみ
		result=true;
	}

	return result;
}
