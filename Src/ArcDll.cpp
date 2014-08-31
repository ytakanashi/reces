//ArcDll.cpp
//統合アーカイバDll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r22 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcDll.h"

#include<iterator>


using namespace sslib;


ArcDll::ArcCallbackProcBase* ArcDll::m_arc_callback_proc=NULL;


ArcDll::ArcDll(const TCHAR* library_name,
			   const TCHAR* library_prefix,
			   const TCHAR* supported_ext,
			   const TCHAR* delimiter):
	ArcDllBase(library_name,library_prefix),
	ArcCommon(),
	m_original_extracting_info(false),
	m_extracting_info_struct_size(0),
	m_processing_info(),
	m_compression_methods(),
	m_method_index(0),
	log_buffer_size(2*1024*1024),
	m_arc_info(),
	m_disable_callback(false){
		str::splitString(&m_supported_ext_list,supported_ext,'.');
		lstrcpy(m_delimiter,delimiter);
}

ArcDll::~ArcDll(){
	if(m_arc_callback_proc){delete m_arc_callback_proc;m_arc_callback_proc=NULL;}
}

//対応している圧縮形式であるか
bool ArcDll::isSupportedMethod(const TCHAR* mhd){
	size_t size=m_compression_methods.size();

	for(m_method_index=0;
		m_compression_methods[m_method_index].mhd!=NULL&&
		lstrcmpi(m_compression_methods[m_method_index].mhd,mhd)!=0&&
		m_method_index<size;
		m_method_index++){}
	return (m_compression_methods[m_method_index].mhd!=NULL);
}

//対応している拡張子であるか
bool ArcDll::isSupportedExtension(const TCHAR* ext){
	return find(m_supported_ext_list.begin(),m_supported_ext_list.end(),str::toLower(ext))!=m_supported_ext_list.end();
}

//対応している書庫であるか
bool ArcDll::isSupportedArchive(const TCHAR* arc_path_orig,const DWORD mode){
	return checkArchive(arc_path_orig,mode)!=0;
}

//対応している書庫であるか(CHECKARCHIVE_RAPID)
bool ArcDll::isSupportedArchiveRapid(const TCHAR* arc_path_orig){
	return checkArchive(arc_path_orig,CHECKARCHIVE_RAPID)!=0;
}

//圧縮形式を取得(その形式に対応している場合のみ)
tstring ArcDll::getCompressionMethod(const TCHAR* arc_path_orig){
	//7-zip32.dll/tar32.dll(<s>/XacRett.dll</s>)はgetArchiveType()を用いて実装
	//パスワードやSFXは含まない
	if(m_compression_methods[0].mhd!=NULL){
		return m_compression_methods[0].mhd;
	}else{
		//圧縮に対応していなければ今のところ何もしない
		return _T("");
	}
}

//パスの区切り文字をそれぞれのライブラリにあったものに置換
void ArcDll::replaceDelimiter(TCHAR* str){
	str::replaceCharacter(str,
						  (*m_delimiter=='/')?'\\':'/',
						  (*m_delimiter=='/')?'/':'\\');
}

//パスの区切り文字をそれぞれのライブラリにあったものに置換
void ArcDll::replaceDelimiter(tstring& str){
	str::replaceCharacter(str,
						  (*m_delimiter=='/')?'\\':'/',
						  (*m_delimiter=='/')?'/':'\\');
}

//パスの区切り文字をそれぞれのライブラリにあったものに置換
void ArcDll::replaceDelimiter(std::list<tstring>* list){
		for(std::list<tstring>::iterator ite=list->begin(),
			end=list->end();
			ite!=end;
			++ite){
			str::replaceCharacter(*ite,
								  (*m_delimiter=='/')?'\\':'/',
								  (*m_delimiter=='/')?'/':'\\');
		}
}

//書庫を開く
bool ArcDll::open(const TCHAR* file_name,const DWORD mode){
	//注意:現バージョンでは一度に複数の書庫を扱わない
	if(m_arc_info.arc_path!=file_name){
		if(!m_arc_info.arc_path.empty())close();
		m_arc_info.arc_path=file_name;
		return openArchive(NULL,file_name,mode)!=NULL;
	}else{
		return true;
	}
}

//書庫を閉じる
bool ArcDll::close(){
	m_arc_info.clear();
	return closeArchive();
}

//指定された書庫ファイルに格納されているファイル数を得ます
//(動作統一の為ArcDllBaseより移植)
int ArcDll::getFileCount(const TCHAR* arc_path){
	createFilesList(arc_path);
	return m_arc_info.file_list.size();
}

//書庫に含まれるファイルのサイズの合計を取得
long long ArcDll::getTotalOriginalSize(const TCHAR* arc_path){
	createFilesList(arc_path);
	return m_arc_info.original_size;
}

//Dllのバージョンを表示
tstring ArcDll::getVersionStr(){
	WORD major_ver,minor_ver;

	getVersion(&major_ver);

	if(!getSubVersion(&minor_ver)){
		//GetSubVersion()が実装されていなければ
		//リソースからバージョンを取得
		DWORD major_ver=0;
		DWORD minor_ver=0;

		if(fileoperation::getFileVersion(name().c_str(),&major_ver,&minor_ver)){
			VariableArgument version(_T("%d.%02d.%02d.%02d"),
									 major_ver>>16,
									 major_ver&0xffff,
									 minor_ver>>16,
									 minor_ver&0xffff);
				return version.get();
		}else{
			if(path::fileExists(name().c_str())){
				//バージョンの取得は出来ないが、存在はする
				return _T("ok");
			}else{
				return _T("not_found");
			}
		}
	}

	//アーカイバDllは上位/下位2ワードに100を掛けた数値を返す
	VariableArgument version(_T("%d.%02d.%02d.%02d"),
							 major_ver/100,
							 major_ver%100,
							 minor_ver/100,
							 minor_ver%100);
	return version.get();
}

//ファイル処理情報を格納
void ArcDll::setExtractingInfo(UINT state,void* arc_info){
	switch(m_extracting_info_struct_size){
		case sizeof(EXTRACTINGINFOEX64):
			//処理済みサイズ
			m_processing_info.done=((LPEXTRACTINGINFOEX64)arc_info)->llWriteSize;
			//ファイルサイズ
			m_processing_info.total=((LPEXTRACTINGINFOEX64)arc_info)->llFileSize;
			//処理中ファイル名
			m_processing_info.file_name=(isUnicodeMode())?
				str::utf82utf16(((LPEXTRACTINGINFOEX64)arc_info)->szSourceFileName):
				str::sjis2utf16(((LPEXTRACTINGINFOEX64)arc_info)->szSourceFileName);
			break;
		case sizeof(EXTRACTINGINFOEX):
			//処理済みサイズ
			m_processing_info.done=((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize;
			//ファイルサイズ
			m_processing_info.total=((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwFileSize;
			//処理中ファイル名
			m_processing_info.file_name=(isUnicodeMode())?
				str::utf82utf16(((LPEXTRACTINGINFOEX)arc_info)->exinfo.szSourceFileName):
				str::sjis2utf16(((LPEXTRACTINGINFOEX)arc_info)->exinfo.szSourceFileName);
			break;
	}
}

ArcDll::ARCDLL_RESULT ArcDll::del(const TCHAR* arc_path_orig,tstring* log_msg){
	return ARCDLL_FAILURE;
}

bool ArcDll::test(const TCHAR* arc_path,tstring* log_msg){
	bool result=checkArchive(arc_path,CHECKARCHIVE_FULLCRC);

	if(log_msg){
		log_msg->assign((result)?_T("正常な書庫です。"):_T("異常な書庫です。"));
	}
	return result;
}

int ArcDll::sendCommands(const TCHAR* commands,tstring* log_msg){
	int dll_ret=-1;

	//実行
	if(log_msg==NULL){
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,commands,&dummy,dummy.length());
	}else{
		dll_ret=execute(NULL,commands,log_msg,log_buffer_size);
	}

	return dll_ret;
}

bool ArcDll::callbackProcV(HWND wnd_handle,UINT msg,UINT state,void* info){
	if(isTerminated()){return false;}
	if(!info||m_disable_callback)return true;
#if 0
	switch(m_extracting_info_struct_size){
		case sizeof(EXTRACTINGINFOEX64):
			if(((LPEXTRACTINGINFOEX64)info)->llFileSize<0||
			   ((LPEXTRACTINGINFOEX64)info)->llWriteSize<0)return true;
			break;
		case sizeof(EXTRACTINGINFOEX):
			if(!((LPEXTRACTINGINFOEX)info)->exinfo.dwFileSize||
			   !((LPEXTRACTINGINFOEX)info)->exinfo.dwWriteSize)return true;
			break;
	}
#endif

	//ファイル処理情報を格納
	if(!m_original_extracting_info){
		setExtractingInfo(state,info);
	}else{
		ArcDll::setExtractingInfo(state,info);
	}

	if(m_processing_info.file_name.empty())return true;

	if(isTerminated()){return false;}

	//通知
	if(!isTerminated()){
		if(m_progress_thread_id){
			misc::thread::post(m_progress_thread_id,WM_UPDATE_PROGRESSBAR,&m_processing_info,reinterpret_cast<void*>(state));
		}
		return true;
	}else{
		return false;
	}
}

BOOL __stdcall ArcDll::callbackProc(HWND wnd_handle,UINT msg,UINT state,void* info){
	return (m_arc_callback_proc)?m_arc_callback_proc->operator()(wnd_handle,msg,state,info):false;
}

//ライブラリから情報を受け取るコールバック関数を設定
bool ArcDll::setArchiveProc(){
	bool result=false;

	if(!(result=setOwnerWindowEx64(NULL,(LPARCHIVERPROC)callbackProc,sizeof(EXTRACTINGINFOEX64)))){
		result=setOwnerWindowEx(NULL,(LPARCHIVERPROC)callbackProc);
		m_extracting_info_struct_size=sizeof(EXTRACTINGINFOEX);
	}else{
		m_extracting_info_struct_size=sizeof(EXTRACTINGINFOEX64);
	}

	if(result){
		m_arc_callback_proc=new ArcCallbackProc<ArcDll>(this,&ArcDll::callbackProcV);
	}

	return result;
}

//ライブラリから情報を受け取るコールバック関数を設定
bool ArcDll::setCallbackProc(unsigned int progress_thread_id,bool original_extracting_info){
	clearCallbackProc();

	m_progress_thread_id=progress_thread_id;
	//ライブラリから受け取った書庫処理状況の情報をそのまま利用する
	m_original_extracting_info=original_extracting_info;
	return setArchiveProc();
}

//コールバック関数の設定を解除
bool ArcDll::clearCallbackProc(){
	bool result=false;

	if(m_arc_callback_proc){
		switch(m_extracting_info_struct_size){
			case sizeof(EXTRACTINGINFOEX64):
				result=killOwnerWindowEx64(NULL);
				break;

			case sizeof(EXTRACTINGINFOEX):
				result=killOwnerWindowEx(NULL);
				break;

			default:
				break;
		}

		delete m_arc_callback_proc;
		m_arc_callback_proc=NULL;
		m_extracting_info_struct_size=0;
		m_progress_thread_id=0;
	}

	return result;
}

//書庫内のすべてのファイルの情報を取得
bool ArcDll::createFilesList(const TCHAR* arc_path){
	//作成済み
	if(m_arc_info.isCreated(arc_path))return true;

	if(!open(arc_path))return false;

	ArcFileSearch fs(this);

	if(fs.first()){
		do{
			fileinfo::FILEINFO* fileinfo=fs.getFileInfo();
			m_arc_info.original_size+=fileinfo->size;
			m_arc_info.file_list.push_back(*fileinfo);
		}while(fs.next());
	}

	closeArchive();

	return !m_arc_info.file_list.empty();
}

//リストにフィルタを適用
void ArcDll::applyFilters(std::list<fileinfo::FILEINFO>* fileinfo_list,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,bool reverse){
	if(filefilter.empty()&&file_ex_filter.empty())return;

	for(std::list<fileinfo::FILEINFO>::iterator ite_fileinfo=fileinfo_list->begin(),
		end=fileinfo_list->end();
		ite_fileinfo!=end;){
		bool matched=fileinfo::matchFilters(*ite_fileinfo,filefilter,file_ex_filter);

		//反転
		if(reverse)matched=!matched;

		if(!matched){
			ite_fileinfo=fileinfo_list->erase(ite_fileinfo);
			continue;
		}
		++ite_fileinfo;
	}
}

//各種構造体のデータより属性を推測
//(各種Dll&書庫によってszAttributeが異なるため保証なし...)
DWORD ArcDll::guessAttribute(const char* attr_str_orig,const char* file_name_orig){
	DWORD result=0;
	tstring file_name((isUnicodeMode())?str::utf82utf16(file_name_orig):str::sjis2utf16(file_name_orig));
	tstring attr_str(str::toLower((isUnicodeMode())?str::utf82utf16(attr_str_orig):str::sjis2utf16(attr_str_orig)));

	if(attr_str.find(_T("r"))!=tstring::npos){
		result|=FILE_ATTRIBUTE_READONLY;
	}

	if(attr_str.find(_T("h"))!=tstring::npos){
		result|=FILE_ATTRIBUTE_HIDDEN;
	}

	if(attr_str.find(_T("s"))!=tstring::npos){
		result|=FILE_ATTRIBUTE_SYSTEM;
	}

	if(attr_str.find(_T("d"))!=tstring::npos||
	   //ライブラリによっては主に使用するデリミタとリスト表示のデリミタが異なる場合有り
	   file_name.rfind(_T("/"))==file_name.length()-1||
	   file_name.rfind(_T("\\"))==file_name.length()-1){
		result|=FILE_ATTRIBUTE_DIRECTORY;
	}

/*
	if(wcschr(attr_str,'A')!=NULL||wcschr(attr_str,'a')!=NULL){
		result|=FILE_ATTRIBUTE_ARCHIVE;
	}
*/

	return result;//(result==0)?FILE_ATTRIBUTE_NORMAL:result;
}

//ディレクトリのタイムスタンプを復元
bool ArcDll::recoverDirectoryTimestamp(const TCHAR* arc_path,const TCHAR* output_dir_orig,bool decode_uesc,bool no_arc_list){
	if(arc_path==NULL||output_dir_orig==NULL)return false;

	struct l{
		static bool recover(const fileinfo::FILEINFO& fileinfo,const TCHAR* output_dir,bool decode_uesc){
			if(!(fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY))return false;

			FILETIME ft={0};
			SYSTEMTIME st={0};

			str::longlong2SYSTEMTIME(&st,fileinfo.date_time);

			if(!::SystemTimeToFileTime(&st,&ft))return false;

			tstring decoded_path(path::addTailSlash(output_dir)+path::removeTailSlash(fileinfo.name));

			if(decode_uesc){
				//Unicodeエスケープがあれば変換
				str::decodeUnicodeEscape(decoded_path,decoded_path.c_str(),false,'#');
			}

			File dir=File(decoded_path.c_str());
			dir.setFileTime(&ft);
			return true;
		}
	};

	tstring output_dir(path::addTailSlash(output_dir_orig));

	std::vector<tstring> recovered_dirs;
	std::vector<tstring> incomplete_dirs;

	if(m_arc_info.isCreated(arc_path)){
		for(std::list<fileinfo::FILEINFO>::iterator ite=m_arc_info.file_list.begin(),
			end=m_arc_info.file_list.end();
			ite!=end;
			++ite){
			if(ite->attr==FILE_ATTRIBUTE_DIRECTORY){
				if(l::recover(*ite,output_dir.c_str(),decode_uesc)){
					recovered_dirs.push_back(output_dir+path::removeTailSlash(ite->name));
				}
			}else{
				DirTimeStamp::addIncompleteDir(output_dir,ite->name,&incomplete_dirs);
			}
		}
	}else{
		if(!open(arc_path))return false;

		ArcFileSearch fs(this);

		if(fs.first()){
			do{
				fileinfo::FILEINFO* fileinfo=fs.getFileInfo();

				if(!no_arc_list){
					m_arc_info.original_size+=fileinfo->size;
					m_arc_info.file_list.push_back(*fileinfo);
				}

				if(fileinfo->attr==FILE_ATTRIBUTE_DIRECTORY){
					if(l::recover(*fileinfo,output_dir.c_str(),decode_uesc)){
						recovered_dirs.push_back(output_dir+path::removeTailSlash(fileinfo->name));
					}
				}else{
					DirTimeStamp::addIncompleteDir(output_dir,fileinfo->name,&incomplete_dirs);
				}
			}while(fs.next());
		}
		closeArchive();
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

	return true;
}

//ファイル名に含まれるUnicodeエスケープシーケンスをデコードする
bool ArcDll::decodeUnicodeEscape(const TCHAR* arc_path,const TCHAR* output_dir,bool ignore_directory_structures,bool no_arc_list){
	if(arc_path==NULL||output_dir==NULL)return false;

	struct l{
		static bool rename(const TCHAR* file_name,const TCHAR* output_dir){
			tstring cur_path(path::addTailSlash(output_dir)+file_name);
			tstring decode_path;

			return str::decodeUnicodeEscape(decode_path,cur_path.c_str(),false,'#')&&
				::fileoperation::renameFile(cur_path.c_str(),decode_path.c_str());
		}
	};

	if(m_arc_info.isCreated(arc_path)){
		for(std::list<fileinfo::FILEINFO>::iterator ite=m_arc_info.file_list.begin(),
			end=m_arc_info.file_list.end();
			ite!=end;
			++ite){
			l::rename(((!ignore_directory_structures)?
					   ite->name.c_str():
					   path::getFileName(ite->name).c_str()),
					  output_dir);
		}
	}else{
		if(!open(arc_path))return false;

		ArcFileSearch fs(this);

		if(fs.first()){
			do{
				fileinfo::FILEINFO* fileinfo=fs.getFileInfo();

				if(no_arc_list){
					m_arc_info.original_size+=fileinfo->size;
					m_arc_info.file_list.push_back(*fileinfo);
				}

				l::rename(((!ignore_directory_structures)?
						   fs.getFileInfo()->name.c_str():
						   path::getFileName(fs.getFileInfo()->name).c_str()),
						  output_dir);
			}while(fs.next());
		}
		closeArchive();
	}
	return true;
}

//リストファイルにファイルリストを出力
void ArcDll::outputFileListToFile(const fileinfo::FILEINFO& fileinfo,const File& list_file,int opt){
	tstring file_path(fileinfo.name);

	if(opt&DECODE_UNICODE_ESCAPE){
		//Unicodeエスケープをデコード
		str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
	}

	if(str::containsWhiteSpace(fileinfo.name.c_str())){
		list_file.writeEx(_T("\"%s\"\r\n"),file_path.c_str());
	}else{
		list_file.writeEx(_T("%s\r\n"),file_path.c_str());
	}
}

//リストファイルにファイルリストを出力
bool ArcDll::outputFileList(const std::list<fileinfo::FILEINFO>& fileinfo_list,const File& list_file,int opt){
	for(std::list<fileinfo::FILEINFO>::const_iterator ite=fileinfo_list.begin(),
		end=fileinfo_list.end();
		ite!=end;++ite){
		outputFileListToFile(*ite,list_file,opt);
	}
	return list_file.getSize()!=0;
}

//コンソールにファイルリストを出力
bool ArcDll::outputFileListToConsole(const fileinfo::FILEINFO& fileinfo,int opt){
	if(isTerminated()){
		return false;
	}

	WORD foreground=Console::LOW_WHITE;

	if(fileinfo.attr&FILE_ATTRIBUTE_SYSTEM){
		foreground=Console::LOW_GREEN;
	}else if(fileinfo.attr&FILE_ATTRIBUTE_HIDDEN){
		foreground=Console::LOW_CYAN;
	}else if(fileinfo.attr&FILE_ATTRIBUTE_READONLY){
		foreground=Console::LOW_RED;
	}else if(fileinfo.attr&FILE_ATTRIBUTE_DIRECTORY){
		foreground=Console::LOW_YELLOW;
	}

	tstring file_path(fileinfo.name);

	if(opt&DECODE_UNICODE_ESCAPE){
		//Unicodeエスケープをデコード
		str::decodeUnicodeEscape(file_path,file_path.c_str(),false,'#');
	}

	app()->stdOut().outputString(foreground,Console::NONE,_T("%s\n"),file_path.c_str());
	return true;
}

//コンソールにファイルリストを出力
bool ArcDll::outputFileList(const std::list<fileinfo::FILEINFO>& fileinfo_list,int opt){
	for(std::list<fileinfo::FILEINFO>::const_iterator ite=fileinfo_list.begin(),
		end=fileinfo_list.end();
		ite!=end&&!isTerminated();
		++ite){
		outputFileListToConsole(*ite,opt);
	}
	return true;
}

//書庫に含まれるファイルの一覧をリストファイルに出力
bool ArcDll::outputFileList(const TCHAR* arc_path,File list_file,int opt){
	if(!list_file.isOpened())return false;

	//作成済み
	if(m_arc_info.isCreated(arc_path)){
		//出力
		outputFileList(m_arc_info.file_list,list_file,opt);
	}else{
		if(!open(arc_path))return false;

		ArcFileSearch fs(this);

		if(fs.first()){
			do{
				fileinfo::FILEINFO* fileinfo=fs.getFileInfo();
				m_arc_info.original_size+=fileinfo->size;
				m_arc_info.file_list.push_back(*fileinfo);

				outputFileListToFile(*fileinfo,list_file,opt);
			}while(fs.next());
		}

		closeArchive();
	}

	return list_file.getSize()!=0;
}

//書庫に含まれるファイルの一覧をコンソールに出力
bool ArcDll::outputFileList(const TCHAR* arc_path,int opt){
	//作成済み
	if(m_arc_info.isCreated(arc_path)){
		//出力
		outputFileList(m_arc_info.file_list,opt);
	}else{
		if(!open(arc_path))return false;

		ArcFileSearch fs(this);

		if(fs.first()){
			do{
				fileinfo::FILEINFO* fileinfo=fs.getFileInfo();
				m_arc_info.original_size+=fileinfo->size;
				m_arc_info.file_list.push_back(*fileinfo);

				outputFileListToConsole(*fileinfo,opt);
			}while(fs.next());
		}

		closeArchive();
	}
	return true;
}

//書庫に含まれるファイルの一覧をリストファイルに出力(フィルタ指定可能)
bool ArcDll::outputFileListEx(const TCHAR* arc_path,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,const File& list_file,int opt){
	if(filefilter.empty()&&file_ex_filter.empty()){
		//フィルタが無ければ
		return outputFileList(arc_path,list_file,opt);
	}

	if(!list_file.isOpened())return false;

	if(!createFilesList(arc_path))return false;

	std::list<fileinfo::FILEINFO> fileinfo_list=m_arc_info.file_list;

	applyFilters(&fileinfo_list,filefilter,file_ex_filter,(opt&REVERSE_FILTER)!=0);

	//出力
	outputFileList(fileinfo_list,list_file,opt);

	return list_file.getSize()!=0;
}

//書庫に含まれるファイルの一覧をコンソールに出力(フィルタ指定可能)
bool ArcDll::outputFileListEx(const TCHAR* arc_path,const fileinfo::FILEFILTER& filefilter,const fileinfo::FILEFILTER& file_ex_filter,int opt){
	if(filefilter.empty()&&file_ex_filter.empty()){
		//フィルタが無ければ
		return outputFileList(arc_path,opt);
	}

	if(!createFilesList(arc_path))return false;

	std::list<fileinfo::FILEINFO> fileinfo_list=m_arc_info.file_list;

	applyFilters(&fileinfo_list,filefilter,file_ex_filter,(opt&REVERSE_FILTER)!=0);

	//出力
	outputFileList(fileinfo_list,opt);

	return true;
}

//作成しようとするディレクトリは不要であるかどうか
bool ArcDll::isRedundantDir(const TCHAR* arc_path,bool check_double_dir,bool check_only_file){
	if(!check_double_dir&&!check_only_file)return false;

	bool result=false;
	bool is_created=m_arc_info.isCreated(arc_path);
	int files_count=0;

	RedundantDir redundant_dir;

	if(is_created){
		for(std::list<fileinfo::FILEINFO>::iterator ite=m_arc_info.file_list.begin(),
			end=m_arc_info.file_list.end();
			ite!=end;
			++ite){
			if(redundant_dir(*ite)){
				break;
			}else{
				continue;
			}
		}
		files_count=m_arc_info.file_list.size();
	}else{
		if(!open(arc_path))return false;

		bool check_end=false;

		ArcFileSearch fs(this);

		if(fs.first()){
			do{
				fileinfo::FILEINFO* fileinfo=fs.getFileInfo();

				m_arc_info.original_size+=fileinfo->size;
				m_arc_info.file_list.push_back(*fileinfo);

				if(!check_end){
					if(redundant_dir(*fileinfo)){
						files_count=m_arc_info.file_list.size();
						//リストを作成しておく為ループ継続
						check_end=true;
					}else{
						continue;
					}
				}
			}while(fs.next());
		}

		closeArchive();

		if(!check_end){
			files_count=m_arc_info.file_list.size();
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
		m_arc_info.doubleDir(true);
	}else if(check_only_file&&
			 files_count==1&&
			 !(m_arc_info.file_list.begin()->attr&FILE_ATTRIBUTE_DIRECTORY)){
		//ファイル単体のみ
		result=true;
		m_arc_info.onlyFile(true);
	}

	return result;
}
