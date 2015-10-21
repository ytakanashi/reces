//Spi.cpp
//Spi操作クラス
//一部の関数のみに対応(書庫関連)

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r30 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"Spi.h"
#include"ArcCfg.h"
#include"Msg.h"

#include<iterator>

using namespace sslib;


namespace{
	//共通パスを取り除く
	tstring excludeCommonPath(const TCHAR* file_path,int delimiter_count){
		tstring file_path_str(file_path);
		for(int i=0;i<delimiter_count;++i)file_path_str=path::removeRootDirectory(file_path_str);
		return file_path_str;
	}
};

//ライブラリを読み込む
bool Spi::load(){
	m_supported_ext_list.clear();
	return Archiver::load();
}

//ライブラリを読み込む
bool Spi::load(const TCHAR* library_name,const TCHAR* library_prefix){
	m_supported_ext_list.clear();
	return Archiver::load(library_name,library_prefix);
}

//spiである[戻り値はプラグインのタイプ]
int Spi::isSusiePlugin(){
	if(!isLoaded())load();

	tstring type(12,'\0');

	if(getPluginInfo(SPI_GET_VERSION,&type,type.length())==0){
		return UNKNOWN;
	}else{
		return (type.substr(type.length()-2)==_T("IN"))?SPI_IN:SPI_AM;
	}
}

//対応している拡張子であるか
bool Spi::isSupportedExtension(const TCHAR* ext){
	if(!isLoaded())load();

	if(m_supported_ext_list.empty()){
		tstring supported_ext;

		for(size_t i=0;;++i){
			tstring tmp(256,'\0');

			if(getPluginInfo(2*i+SPI_GET_EXT,&tmp,tmp.length())==0)break;

			if(tmp==_T(""))break;
			supported_ext+=tmp+_T(";");

			tmp.clear();
		}

		if(supported_ext.empty())return false;

		str::splitString(&m_supported_ext_list,str::toLower(supported_ext).c_str(),';');
		m_supported_ext_list.sort();
		m_supported_ext_list.unique();
	}

	for(std::list<tstring>::iterator ite=m_supported_ext_list.begin(),
		end=m_supported_ext_list.end();
		ite!=end;
		++ite){
		if(str::matchWildcards(ext,ite->c_str())){
			return true;
		}
	}
	return false;
}

//対応している書庫か
bool Spi::isSupportedArchive(const TCHAR* arc_path,int mode){
	return isSupportedExtension((tstring(_T("*."))+=str::toLower(path::getExtension(arc_path))).c_str());
}

//プラグインについての情報を取得
tstring Spi::getInformation(){
	if(!isLoaded())load();

	tstring about(256,'\0');

	getPluginInfo(SPI_GET_ABOUT,&about,about.length());

	return about;
}

//設定ダイアログを表示
bool Spi::configurationDialog(HWND wnd_handle){
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

Spi::ARC_RESULT Spi::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	return ARC_NOT_IMPLEMENTED;
}

Spi::ARC_RESULT Spi::extract(const TCHAR* arc_path,const TCHAR* output_dir_orig,tstring* log_msg){
	if(!isLoaded())load();

	ARC_RESULT result=ARC_FAILURE;
	tstring output_dir(path::addTailSlash(output_dir_orig));

	HLOCAL h_info=NULL;

	if(getArchiveInfo(arc_path,0,SPI_INPUT_FILE,&h_info)!=SPI_ERROR_SUCCESS){
		return result;
	}

	fileInfo* arc_info=reinterpret_cast<fileInfo*>(::LocalLock(h_info));
	fileInfo* arc_info_header=arc_info;

	if(!arc_info){
		return result;
	}

	int file_count=getFileCount(arc_path);
	int delimiter_count=(CFG.compress.exclude_base_dir!=0)?countDelimiter(arc_path,m_arc_info):0;

	if(log_msg!=NULL&&
	   !CFG.no_display.no_log){
		log_msg->append(_T("\n"));
	}

	if(CFG.compress.exclude_base_dir>0&&
	   delimiter_count>CFG.compress.exclude_base_dir){
		delimiter_count=CFG.compress.exclude_base_dir;
	}

	result=ARC_SUCCESS;
	msg::info(_T("'%s'を解凍しています...\n\n"),arc_path);

	long now=0,last=0;

	for(;arc_info->method[0]&&!IS_TERMINATED&&!ARCCFG->m_password_input_cancelled;++arc_info){
		tstring file_path=str::sjis2utf16(arc_info->path);
		tstring file_name=str::sjis2utf16(arc_info->filename);

		if(!filter::match(fileinfo::FILEINFO(file_path+file_name,
											arc_info->filesize,
											0,
											((long long)arc_info->timestamp+11644473600)*10000000),
						 CFG.general.filefilter,
						 CFG.general.file_ex_filter))continue;

		if(!CFG.general.ignore_directory_structures){
			if(file_name.find_last_of(_T("\\/"))==file_name.length()-1){
				//ディレクトリ名に含まれるUnicodeエスケープシーケンスをデコードする
				tstring decoded_path((output_dir+file_path)+=file_name);

				//共通パスを取り除く
				if(CFG.compress.exclude_base_dir!=0){
					decoded_path=output_dir+excludeCommonPath(decoded_path.substr(output_dir.length()).c_str(),delimiter_count);
				}

				fileoperation::createDirectory(decoded_path.c_str());
				continue;
			}else{
				//相対パスがpathの他にfilenameに代入されるものもあるため
				tstring dir_name=file_path+file_name;
				tstring parent_dir=path::getParentDirectory(dir_name);

				if(dir_name!=parent_dir){
					tstring decoded_path(output_dir+parent_dir);
					//共通パスを取り除く
					if(CFG.compress.exclude_base_dir!=0){
						decoded_path=output_dir+excludeCommonPath(decoded_path.substr(output_dir.length()).c_str(),delimiter_count);
					}

					fileoperation::createDirectory(decoded_path.c_str());
				}
			}
		}

		if(m_progress_thread_id){
			SYSTEMTIME st={};
			::GetSystemTime(&st);
			now=st.wSecond*1000+st.wMilliseconds;
			if(now-last>100){
				ARC_PROCESSING_INFO spi_processing_info((arc_info-arc_info_header)+1,file_count,file_path+file_name);

				misc::thread::post(m_progress_thread_id,WM_UPDATE_PROGRESSBAR,reinterpret_cast<void*>(&spi_processing_info));
			}
		}

		HLOCAL file;
		File dest;
		tstring err_msg;

		dest.open((output_dir+
				   excludeCommonPath((((!CFG.general.ignore_directory_structures)?
									   file_path+file_name:
									   file_name)).c_str(),
									 delimiter_count)).c_str(),
				  OPEN_ALWAYS);

		if(!dest.isOpened())continue;

		{
			int ret_code=getFile(arc_path,arc_info->position,&file,SPI_INPUT_FILE|SPI_OUTPUT_MEMORY,NULL,0);

			if(log_msg!=NULL&&
			   !CFG.no_display.no_log){
				if(getErrorMessage(&err_msg,ret_code)){
					log_msg->append(((err_msg+=_T("   "))+=file_name)+=_T("\n"));
				}
			}
			if(!(result==ARC_SUCCESS&&SPI_ERROR_SUCCESS==ret_code))result=ARC_FAILURE;
		}

		const void* data=::LocalLock(file);

		dest.write(data,arc_info->filesize);

		if(arc_info->timestamp){
			FILETIME ft={};

			ft=strex::lltoft(((long long)arc_info->timestamp+11644473600)*10000000);

			dest.setFileTime(&ft);
		}

		tstring cur_path(dest.filepath());

		dest.close();

		::LocalUnlock(file);
		::LocalFree(file);
	}

	std::vector<tstring> recovered_dirs;
	std::vector<tstring> incomplete_dirs;

	//ディレクトリのタイムスタンプを復元
	if(!CFG.general.ignore_directory_structures&&
	   CFG.extract.directory_timestamp){
		bool created=!m_arc_info.empty();

		if(!created){
			m_arc_info.resize(1);
		}

		for((!created)?arc_info=arc_info_header:0;
			!IS_TERMINATED&&((!created)?arc_info->method[0]:1);
			(!created)?++arc_info:0){

			if(!created){
				tstring file_name=str::sjis2utf16(arc_info->filename);

				m_arc_info.back()=fileinfo::FILEINFO(str::sjis2utf16(arc_info->path)+=file_name,
													 arc_info->filesize,
													 (file_name.find_last_of(_T("\\/"))==file_name.length()-1)?FILE_ATTRIBUTE_DIRECTORY:0,
													 ((long long)arc_info->timestamp+11644473600)*10000000);
			}

			for(size_t i=0,size=m_arc_info.size();i<size&&!IS_TERMINATED;++i){
				if(m_arc_info[i].attr&FILE_ATTRIBUTE_DIRECTORY){
					if(m_arc_info[i].date_time){
						FILETIME ft={};

						ft=strex::lltoft(m_arc_info[i].date_time);

						//共通パスを取り除く
						tstring cur_path(output_dir+excludeCommonPath(m_arc_info[i].name.c_str(),delimiter_count));

						if(path::removeTailSlash(cur_path)==path::removeTailSlash(output_dir))continue;

						File dest(cur_path.c_str());

						dest.setFileTime(&ft);

						recovered_dirs.push_back(path::removeTailSlash(cur_path));
					}
				}else{
					dirtimestamp::addIncompleteDir(output_dir,excludeCommonPath(m_arc_info[i].name.c_str(),delimiter_count),&incomplete_dirs);
				}
			}
			if(created)break;
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
			dirtimestamp::recover(incomplete_dirs[i].c_str(),arc_ft,recovered_dirs);
		}
	}

	::LocalUnlock(h_info);
	::LocalFree(h_info);

	m_arc_info.clear();
	return result;
}

Spi::ARC_RESULT Spi::list(const TCHAR* arc_path){
	if(!isLoaded())load();
	ARC_RESULT result=ARC_FAILURE;

	HLOCAL h_info=NULL;

	if(getArchiveInfo(arc_path,0,SPI_INPUT_FILE,&h_info)!=SPI_ERROR_SUCCESS){
		return result;
	}

	Spi::fileInfo* arc_info=reinterpret_cast<fileInfo*>(::LocalLock(h_info));

	if(!arc_info){
		return result;
	}

	if(!CFG.output_file_list.api_mode){
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("\n   Date      Time         Size Name\n"));
		STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("---------- -------- ---------- ------------------------\n"));
	}

	for(;arc_info->method[0]&&!IS_TERMINATED;++arc_info){
		tstring file_path=str::sjis2utf16(arc_info->path);
		tstring file_name=str::sjis2utf16(arc_info->filename);
		tstring full_path(file_path+file_name);

		if(!filter::match(fileinfo::FILEINFO(file_path+file_name,
											arc_info->filesize,
											(file_name.find_last_of(_T("\\/"))==file_name.length()-1)?FILE_ATTRIBUTE_DIRECTORY:0,
											((long long)arc_info->timestamp+11644473600)*10000000),
						 CFG.general.filefilter,
						 CFG.general.file_ex_filter))continue;

		if(!CFG.output_file_list.api_mode){
			if(arc_info->timestamp){
				FILETIME ft={},tmp={};
				SYSTEMTIME st;

				tmp=strex::lltoft(((long long)arc_info->timestamp+11644473600)*10000000);

				if(!::FileTimeToLocalFileTime(&tmp,&ft)){
					continue;
				}
				if(!::FileTimeToSystemTime(&ft,&st)){
					continue;
				}

				STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%04u/%02u/%02u %02u:%02u:%02u "),
											 st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			}else{
				//タイムスタンプが存在しない
				STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("                    "));
			}

			STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%10d %s\n"),
										 arc_info->filesize,
										 full_path.c_str());
		}else{
			if(file_name.find_last_of(_T("\\/"))==file_name.length()-1){
				//ディレクトリ
				STDOUT.outputString(Console::LOW_YELLOW,Console::NONE,_T("%s\n"),
									  full_path.c_str());
			}else{
				STDOUT.outputString(_T("%s\n"),
									  full_path.c_str());
			}
		}
	}
	::LocalUnlock(h_info);
	::LocalFree(h_info);

	m_arc_info.clear();
	result=ARC_SUCCESS;
	return result;
}

Spi::ARC_RESULT Spi::test(const TCHAR* arc_path){
	return ARC_NOT_IMPLEMENTED;
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

		m_arc_info.push_back(fileinfo::FILEINFO(str::sjis2utf16(arc_info->path)+=file_name,
													 arc_info->filesize,
													 (file_name.find_last_of(_T("\\/"))==file_name.length()-1)?FILE_ATTRIBUTE_DIRECTORY:0,
													 ((long long)arc_info->timestamp+11644473600)*10000000));
	}
	::LocalUnlock(h_info);
	::LocalFree(h_info);
	return !m_arc_info.empty();
}

//作成しようとするディレクトリは不要であるかどうか
bool Spi::isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file,tstring* root_dir){
	if(!check_double_dir&&!check_only_file)return false;

	bool result=false;
	int files_count=0;

	RedundantDir redundant_dir;

	if(!m_arc_info.empty()){
		for(std::vector<fileinfo::FILEINFO>::iterator ite=m_arc_info.begin(),
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

			m_arc_info.push_back(fileinfo::FILEINFO(str::sjis2utf16(arc_info->path)+=file_name,
														 arc_info->filesize,
														 (file_name.find_last_of(_T("\\/"))==file_name.length()-1)?FILE_ATTRIBUTE_DIRECTORY:0,
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

	if(root_dir)*root_dir=redundant_dir.root_dir();

	return result;
}
