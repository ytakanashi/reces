//Wcx.cpp
//wcx操作クラス
//一部の関数のみに対応

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"Wcx.h"
#include"ArcCfg.h"

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

//wcxである
bool Wcx::isWcx(){
	if(!isLoaded())load();

	//てきとー
	return getAddress(_T("CloseArchive"))!=NULL&&
		(getAddress(_T("ReadHeader"))!=NULL||
		 getAddress(_T("ReadHeaderEx"))!=NULL||
		 getAddress(_T("ReadHeaderExW"))!=NULL);
}

//対応している拡張子であるか
bool Wcx::isSupportedExtension(const TCHAR* ext){
	//プラグインから対応拡張子を知ることが出来ない。
	return false;
}

//対応している書庫か
bool Wcx::isSupportedArchive(const TCHAR* arc_path,int mode){
	if(!isLoaded())load();
	bool result=false;

	m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode());
	result=openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS;
	closeArchive();
	SAFE_DELETE(m_open_arc_data);

	return result;
}

//設定ダイアログを表示
bool Wcx::configurationDialog(HWND wnd_handle){
	return configurePacker(wnd_handle,GetModuleHandle(NULL));
}

int Wcx::getFileCount(const TCHAR* arc_path){
	if(!isLoaded())load();
	int count=0;

	m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode());
	if(!(openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS)){
		closeArchive();
		SAFE_DELETE(m_open_arc_data);
		return count;
	}

	m_header_data=new HEADER_DATA(getHeaderDataMode());
	for(;!readHeader()&&!isTerminated();processFile(PK_SKIP))++count;
	SAFE_DELETE(m_header_data);
	closeArchive();
	SAFE_DELETE(m_open_arc_data);
	return count;
}

Wcx::ARC_RESULT Wcx::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	return ARC_FAILURE;
}

Wcx::ARC_RESULT Wcx::extract(const TCHAR* arc_path,const TCHAR* output_dir_orig,tstring* log_msg){
	if(!isLoaded())load();

	ARC_RESULT result=ARC_FAILURE;
	tstring output_dir(path::addTailSlash(output_dir_orig));
	int file_count=getFileCount(arc_path);
	int delimiter_count=(CFG.compress.exclude_base_dir!=0)?countDelimiter(arc_path,m_arc_info):0;

	m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode(),PK_OM_EXTRACT);
	if(!(openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS)){
		closeArchive();
		SAFE_DELETE(m_open_arc_data);
		return result;
	}

	m_header_data=new HEADER_DATA(getHeaderDataMode());

	if(log_msg!=NULL&&
	   !CFG.no_display.no_log){
		log_msg->append(_T("\n"));
	}

	if(CFG.compress.exclude_base_dir>0&&
	   delimiter_count>CFG.compress.exclude_base_dir){
		delimiter_count=CFG.compress.exclude_base_dir;
	}

	result=ARC_SUCCESS;
	if(!CFG.no_display.no_information)STDOUT.outputString(_T("'%s'を解凍しています...\n\n"),arc_path);

	for(int i=1;!readHeader()&&!isTerminated();++i){
		int ret_code=E_BAD_DATA;

		if(!matchFilters(data(),
						 CFG.general.filefilter,
						 CFG.general.file_ex_filter)){
			processFile(PK_SKIP);
			continue;
		}

		tstring file_path(m_header_data->filename());
		tstring file_name(path::getFileName(m_header_data->filename()));

		//ファイル名に含まれるUnicodeエスケープシーケンスをデコードする
		if(CFG.general.decode_uesc){
			str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
			str::decodeUnicodeEscape(file_name,file_name.c_str(),false,'#');
		}

		if(!CFG.general.ignore_directory_structures){
			if(m_header_data->attr()&FILE_ATTRIBUTE_DIRECTORY){
				tstring dir_path(output_dir+file_path);
				//共通パスを取り除く
				if(CFG.compress.exclude_base_dir!=0){
					dir_path=output_dir+excludeCommonPath(dir_path.substr(output_dir.length()).c_str(),delimiter_count);
				}
				fileoperation::createDirectory(dir_path.c_str());
				processFile(PK_SKIP);
				continue;
			}
		}

		if(m_progress_thread_id){
			ARC_PROCESSING_INFO wcx_processing_info(i,file_count,file_path);

			misc::thread::post(m_progress_thread_id,WM_UPDATE_PROGRESSBAR,reinterpret_cast<void*>(&wcx_processing_info));
		}

		tstring err_msg;

		{
			ret_code=E_BAD_DATA;

			{
				tstring path((!CFG.general.ignore_directory_structures)?
							 output_dir+excludeCommonPath(file_path.c_str(),delimiter_count):
							 output_dir+file_name);

				if(!CFG.general.ignore_directory_structures){
					fileoperation::createDirectory(path::getParentDirectory(path).c_str());
				}
				ret_code=processFile(PK_EXTRACT,NULL,path.c_str());
			}
			if(log_msg!=NULL&&
			   !CFG.no_display.no_log){
				if(getErrorMessage(&err_msg,ret_code)){
					log_msg->append(((err_msg+=_T("   "))+=file_name)+=_T("\n"));
				}
			}
			if(!(result==ARC_SUCCESS&&E_SUCCESS==ret_code))result=ARC_FAILURE;
		}
	}

	SAFE_DELETE(m_header_data);
	closeArchive();
	SAFE_DELETE(m_open_arc_data);

	std::vector<tstring> recovered_dirs;
	std::vector<tstring> incomplete_dirs;

	//ディレクトリのタイムスタンプを復元
	if(!CFG.general.ignore_directory_structures&&
	   CFG.extract.directory_timestamp){
		bool created=!m_arc_info.empty();

		if(!created){
			m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode(),PK_OM_EXTRACT);
			if(!(openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS)){
				closeArchive();
				SAFE_DELETE(m_open_arc_data);
				return result;
			}
			m_header_data=new HEADER_DATA(getHeaderDataMode());
			m_arc_info.resize(1);
		}

		for(;!isTerminated()&&((!created)?!readHeader():1);
			(!created)?processFile(PK_SKIP):0){
			if(!created){
				m_arc_info.back()=data();
			}
			for(size_t i=0,size=m_arc_info.size();i<size&&!isTerminated();++i){
				if(m_arc_info[i].attr&FILE_ATTRIBUTE_DIRECTORY){
					if(m_arc_info[i].date_time){
						FILETIME tmp={},ft={};

						tmp=str::lltoft(m_arc_info[i].date_time);
						//UTCに変換
						if(::LocalFileTimeToFileTime(&tmp,&ft)){
							tstring cur_path(output_dir+excludeCommonPath(m_arc_info[i].name.c_str(),delimiter_count));

							if(path::removeTailSlash(cur_path)==path::removeTailSlash(output_dir))continue;

							if(CFG.general.decode_uesc){
								//Unicodeエスケープがあれば変換
								str::decodeUnicodeEscape(cur_path,cur_path.c_str(),false,'#');
							}

							File dest(cur_path.c_str());

							dest.setFileTime(&ft);

							recovered_dirs.push_back(path::removeTailSlash(cur_path));
						}
					}
				}else{
					DirTimeStamp::addIncompleteDir(output_dir,excludeCommonPath(m_arc_info[i].name.c_str(),delimiter_count),&incomplete_dirs);
				}
			}
			if(created)break;
		}
		if(!created){
			SAFE_DELETE(m_header_data);
			closeArchive();
			SAFE_DELETE(m_open_arc_data);
			m_arc_info.clear();
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

	return result;
}

Wcx::ARC_RESULT Wcx::list(const TCHAR* arc_path){
	if(!isLoaded())load();
	ARC_RESULT result=ARC_FAILURE;

	m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode());
	result=(openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS)?ARC_SUCCESS:ARC_FAILURE;

	if(result==ARC_FAILURE){
		SAFE_DELETE(m_open_arc_data);
		return result;
	}

	m_header_data=new HEADER_DATA(getHeaderDataMode());

	if(!CFG.output_file_list.api_mode){
		STDOUT.outputString(_T("\n   Date      Time                  Size Name\n"));
		STDOUT.outputString(_T("---------- -------- ------------------- ------------------------\n"));
	}

	for(;!readHeader()&&!isTerminated();processFile(PK_SKIP)){
		fileinfo::FILEINFO fileinfo=data();

		if(!matchFilters(fileinfo,
						 CFG.general.filefilter,
						 CFG.general.file_ex_filter))continue;

		if(!CFG.output_file_list.api_mode){
			if(fileinfo.date_time){
				FILETIME ft=str::lltoft(fileinfo.date_time);
				SYSTEMTIME st;

				if(!::FileTimeToSystemTime(&ft,&st))continue;

				STDOUT.outputString(_T("%04u/%02u/%02u %02u:%02u:%02u "),
									  st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			}

			tstring file_path(m_header_data->filename());

			if(CFG.general.decode_uesc){
				//Unicodeエスケープをデコード
				str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
			}

			STDOUT.outputString(_T("%19I64d %s\n"),
								  m_header_data->filesize(),
								  file_path.c_str());
		}else{
			tstring file_path(m_header_data->filename());

			if(CFG.general.decode_uesc){
				//Unicodeエスケープをデコード
				str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
			}

			if(m_header_data->attr()&FILE_ATTRIBUTE_DIRECTORY){
				//ディレクトリ
				STDOUT.outputString(Console::LOW_YELLOW,Console::NONE,_T("%s\n"),
									  file_path.c_str());
			}else{
				STDOUT.outputString(_T("%s\n"),
									  file_path.c_str());
			}
		}
	}



	SAFE_DELETE(m_header_data);
	closeArchive();
	SAFE_DELETE(m_open_arc_data);

	result=ARC_SUCCESS;
	return result;
}

Wcx::ARC_RESULT Wcx::test(const TCHAR* arc_path){
	if(!isLoaded())load();
	ARC_RESULT result=ARC_FAILURE;

	m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode(),PK_OM_EXTRACT);
	result=(openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS)?ARC_SUCCESS:ARC_FAILURE;

	if(result==ARC_FAILURE){
		SAFE_DELETE(m_open_arc_data);
		return result;
	}

	m_header_data=new HEADER_DATA(getHeaderDataMode());

	result=ARC_SUCCESS;

	while(!readHeader()&&!isTerminated()){
		int ret_code=E_BAD_DATA;

		if(m_header_data->attr()&FILE_ATTRIBUTE_DIRECTORY){
			//ディレクトリ
			processFile(PK_SKIP);
		}else{
			ret_code=processFile(PK_TEST);
			if(!CFG.no_display.no_log){
				tstring err_msg;

				if(getErrorMessage(&err_msg,ret_code)){
					STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%s   %s\n"),err_msg.c_str(),path::getFileName(m_header_data->filename()).c_str());
				}
			}
			if(!(result==ARC_SUCCESS&&E_SUCCESS==ret_code))result=ARC_FAILURE;
		}
	}



	SAFE_DELETE(m_header_data);
	closeArchive();
	SAFE_DELETE(m_open_arc_data);

	return result;
}

//エラーメッセージを取得
bool Wcx::getErrorMessage(tstring* msg,int code){
	if(msg==NULL)return false;

	switch(code){
		case E_SUCCESS:
			msg->assign(_T("正常終了"));
			break;
		case E_END_ARCHIVE:
			break;
		case E_NO_MEMORY:
			msg->assign(_T("メモリーが確保出来ない"));
			break;
		case E_BAD_DATA:
			msg->assign(_T("CRCエラー"));
			break;
		case E_BAD_ARCHIVE:
			msg->assign(_T("データが壊れている"));
			break;
		case E_UNKNOWN_FORMAT:
			msg->assign(_T("未知のフォーマット"));
			break;
		case E_EOPEN:
			msg->assign(_T("ファイルオープンエラー"));
			break;
		case E_ECREATE:
			msg->assign(_T("ファイル作成エラー"));
			break;
		case E_ECLOSE:
			msg->assign(_T("ファイルクローズエラー"));
			break;
		case E_EREAD:
			msg->assign(_T("ファイル読み込みエラー"));
			break;
		case E_EWRITE:
			msg->assign(_T("ファイル書き込みエラー"));
			break;
		case E_SMALL_BUF:
			msg->assign(_T("バッファが小さい"));
			break;
		case E_EABORTED:
			msg->assign(_T("ユーザーにより処理が中止された"));
			break;
		case E_NO_FILES:
			msg->assign(_T("ファイルがない"));
			break;
		case E_TOO_MANY_FILES:
			msg->assign(_T("ファイルの数が多すぎる"));
			break;
		case E_NOT_SUPPORTED:
			msg->assign(_T("未知のフォーマット"));
			break;
		default:
			break;
	}
	return true;
}

//ヘッダーから情報を取得
fileinfo::FILEINFO Wcx::data(){
	assert(m_header_data!=NULL);

	FILETIME ft={};
	bool err_ts=true;

	if(m_header_data->timestamp()&&
	   ::DosDateTimeToFileTime(m_header_data->timestamp()>>16,m_header_data->timestamp()&0xffff,&ft)){
		err_ts=false;
	}

	return fileinfo::FILEINFO(m_header_data->filename(),
							  m_header_data->filesize(),
							  m_header_data->attr(),
							  (!err_ts)?str::fttoll(ft):-1);
}

//書庫内のすべてのファイルの情報を取得
bool Wcx::createFilesList(const TCHAR* arc_path){
	//作成済み
	if(!m_arc_info.empty())return true;

	if(!isLoaded())load();

	m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode());
	if(!(openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS)){
		closeArchive();
		SAFE_DELETE(m_open_arc_data);
		return false;
	}

	m_header_data=new HEADER_DATA(getHeaderDataMode());

	for(;!readHeader()&&!isTerminated();processFile(PK_SKIP)){
		m_arc_info.push_back(data());
	}
	SAFE_DELETE(m_header_data);
	closeArchive();
	SAFE_DELETE(m_open_arc_data);
	return !m_arc_info.empty();
}

//作成しようとするディレクトリは不要であるかどうか
bool Wcx::isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file){
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

		m_open_arc_data=new OPEN_ARC_DATA(arc_path,isUnicodeMode());
		if(!(openArchive()!=NULL&&m_open_arc_data->result()==E_SUCCESS)){
			closeArchive();
			SAFE_DELETE(m_open_arc_data);
			return result;
		}

		m_header_data=new HEADER_DATA(getHeaderDataMode());

		bool check_end=false;

		for(;!readHeader()&&!isTerminated();processFile(PK_SKIP)){
			m_arc_info.push_back(data());
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

		SAFE_DELETE(m_header_data);
		closeArchive();
		SAFE_DELETE(m_open_arc_data);

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
