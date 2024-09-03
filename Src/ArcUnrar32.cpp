//ArcUnrar32.cpp
//Unrar32.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r34 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcUnrar32.h"
#include"Hook/HookArchiverDialog.h"
#include"ArcCfg.h"

using namespace sslib;


namespace{
namespace hook{
	//ArcUnrar32のコンストラクタで代入すること
	ArcUnrar32* this_ptr=NULL;
	bool no_information=false;
	long long progress_done=0,progress_total=0;

	typedef BOOL(WINAPI*pShowWindow)(HWND,int);
	typedef HWND(WINAPI*pCreateDialogParamW)(HINSTANCE,LPCTSTR,HWND,DLGPROC,LPARAM);
	typedef BOOL(WINAPI*pSetDlgItemTextW)(HWND,int,LPCWSTR);

	pShowWindow org_pShowWindow;
	pCreateDialogParamW org_pCreateDialogParamW;
	pSetDlgItemTextW org_pSetDlgItemTextW;
	HWND extracting_wnd_handle=NULL;

	BOOL WINAPI showWindow(HWND hWnd,int nCmdShow){
		int state=nCmdShow;

		if(hWnd==extracting_wnd_handle)state=SW_HIDE;
		return org_pShowWindow(hWnd,state);
	}

	HWND WINAPI createDialogParamW(HINSTANCE hInstance,LPCTSTR lpTemplateName,HWND hWndParent,DLGPROC lpDialogFunc,LPARAM lParamInit){
		HWND cur_wnd=::GetForegroundWindow();
		HWND result=org_pCreateDialogParamW(hInstance,lpTemplateName,hWndParent,lpDialogFunc,lParamInit);

		if(::GetDlgItem(result,1009)){
			extracting_wnd_handle=result;
			::SetForegroundWindow(cur_wnd);
		}
		return result;
	}

	BOOL WINAPI setDlgItemTextW(HWND hWnd,int nIDDlgItem,LPCWSTR lpString){
		if(hWnd==extracting_wnd_handle){
			if(nIDDlgItem==1009){
				//ファイル
				TCHAR buffer[MAX_PATH]={};

				::GetWindowText(::GetDlgItem(extracting_wnd_handle,nIDDlgItem),buffer,MAX_PATH);
				this_ptr->preCallback(buffer,NULL);
			}
#if 0
			else if(nIDDlgItem==1010)
				//ディレクトリは1009に表示されないため、1010でカウント
				//...すべきですがややこしいのでファイルのみカウント
#endif
		}
		return org_pSetDlgItemTextW(hWnd,nIDDlgItem,lpString);
	}

	void init(){
		extracting_wnd_handle=NULL;

		//ShowWindow
		org_pShowWindow=(pShowWindow)GetProcAddress(GetModuleHandleA("user32"),"ShowWindow");
		//CreateDialogParamW
		org_pCreateDialogParamW=(pCreateDialogParamW)GetProcAddress(GetModuleHandleA("user32"),"CreateDialogParamW");
		//SetDlgItemTextW
		if(!no_information)org_pSetDlgItemTextW=(pSetDlgItemTextW)GetProcAddress(GetModuleHandleA("user32"),"SetDlgItemTextW");
	}

	void install(bool no_information_=false){
		no_information=no_information_;
		init();

		//ShowWindow
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pShowWindow),
						 reinterpret_cast<PROC>(showWindow),
						 this_ptr->module());

		//CreateDialogParamW
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pCreateDialogParamW),
						 reinterpret_cast<PROC>(createDialogParamW),
						 this_ptr->module());

		//SetDlgItemTextW
		if(!no_information)
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pSetDlgItemTextW),
						 reinterpret_cast<PROC>(setDlgItemTextW),
						 this_ptr->module());
	}

	void uninstall(){
		//ShowWindow
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(showWindow),
						 reinterpret_cast<PROC>(org_pShowWindow),
						 this_ptr->module());

		//CreateDialogParamW
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(createDialogParamW),
						 reinterpret_cast<PROC>(org_pCreateDialogParamW),
						 this_ptr->module());

		//SetDlgItemTextW
		if(!no_information)
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(setDlgItemTextW),
						 reinterpret_cast<PROC>(org_pSetDlgItemTextW),
						 this_ptr->module());
	}
}
}

ArcUnrar32::ArcUnrar32():
	ArcDll(
#ifndef _WIN64
			_T("Unrar32"),
#else
			_T("Unrar64j"),
#endif
			_T("Unrar"),
			_T("rar"),
			_T("\\")){
		hook::this_ptr=this;
		COMPRESSION_FORMAT format[]={
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_formats.assign(format,format+ARRAY_SIZEOF(format));
}

//対応している書庫であるか
bool ArcUnrar32::isSupportedArchive(const TCHAR* arc_path_orig,int mode){
	bool result=false;

	tstring arc_path(arc_path_orig);
	replaceDelimiter(arc_path);

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
		result=checkArchive(arc_path.c_str(),mode);
	}while(!IS_TERMINATED&&
		   !ARCCFG->m_password_input_cancelled&&
		   !result&&
		   ARCCFG->m_hook_dialog_type==HOOK_UNRAR32_PASSWORD);

	return result;


#if 0
	tstring cmd_line(format(_T("%s %s %s %s"),
							_T("-l"),
							_T("-q"),
							_T("--"),
							path::quote(arc_path).c_str()));

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	bool use_password=!CFG.general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=CFG.general.password_list.begin();
	std::list<tstring>::iterator password_list_end=CFG.general.password_list.end();

	int dll_ret=-1;
	m_processing_info.clear();

	do{
		if(use_password&&ite_password_list!=password_list_end){
			CFG.general.password=*ite_password_list;
			++ite_password_list;
		}else{
			CFG.general.password.clear();
		}

		//実行
		tstring dummy(1,'\0');

		dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
	}while(!IS_TERMINATED&&
		   dll_ret==ERROR_HEADER_BROKEN);

	return dll_ret==0;
#endif
}

//書庫をテスト
ArcUnrar32::ARC_RESULT ArcUnrar32::test(const TCHAR* arc_path){
	bool result=false;

	if(isSupportedArchive(arc_path)){
		tstring arc_path_str(arc_path);
		replaceDelimiter(arc_path_str);

		result=checkArchive(arc_path_str.c_str(),CHECKARCHIVE_FULLCRC|CHECKARCHIVE_ALL);
	}

	if(!CFG.no_display.no_log){
		STDOUT.outputStringF(Console::LOW_GREEN,Console::NONE,_T("%s\n"),
								   (result)?_T("正常な書庫です。"):_T("異常な書庫です。"));
	}

	return (result)?ARC_SUCCESS:ARC_FAILURE;
}

ArcUnrar32::ARC_RESULT ArcUnrar32::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	return ARC_NOT_IMPLEMENTED;
}

ArcUnrar32::ARC_RESULT ArcUnrar32::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	hook::progress_done=hook::progress_total=0;

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
	tstring list_file_path;
	File list_file;

	if(use_filter){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("rar"),ARCCFG->m_list_temp_dir.c_str());
		//BOMなしファイルを出力
		if(!list_file.open(list_file_path.c_str(),
						   OPEN_ALWAYS,
						   GENERIC_WRITE,
						   0,
						   (isUnicodeMode())?static_cast<File::CODEPAGE>(File::UTF8|File::NO_BOM):File::SJIS)){
			return ARC_CANNOT_OPEN_LISTFILE;
		}

		//リストファイルに解凍対象ファイルのみ出力
		if(!outputFileListEx(arc_path_str.c_str(),
							 CFG.general.filefilter,
							 CFG.general.file_ex_filter,
							 0,
							 &list_file)){
			return ARC_NO_MATCHES_FOUND;
		}
		list_file.close();
	}

	if(IS_TERMINATED)return ARC_FAILURE;

	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		hook::progress_total=getFileCount(arc_path_str.c_str());
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

	tstring cmd_line(format(_T("%s %s %s %s %s %s"),
							(!CFG.general.ignore_directory_structures)?_T("-x"):_T("-e"),
							_T("-s -o -y"),
							//-s     : filespec の比較を厳密に行います

//							_T("-q"),
							_T(""),

							_T("--"),

							path::quote(arc_path_str).c_str(),

							path::quote(output_dir_str).c_str()));

	if(use_filter){
		cmd_line.append(format(_T(" @%s"),
							   path::quote(list_file_path).c_str()));
	}

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());


	msg::info(_T("'%s'を解凍しています...\n\n"),arc_path);

	bool use_password=!CFG.general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=CFG.general.password_list.begin();
	std::list<tstring>::iterator password_list_end=CFG.general.password_list.end();

	hook::install(CFG.no_display.no_information||STDOUT.isRedirected());

	int dll_ret=-1;
	m_processing_info.clear();

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
		   dll_ret>=ERROR_START&&
		   (++ite_password_list)!=password_list_end);

	msg::info(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);

	hook::uninstall();

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

ArcUnrar32::ARC_RESULT ArcUnrar32::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	if(CFG.output_file_list.api_mode){
		outputFileListEx(arc_path_str.c_str(),CFG.general.filefilter,CFG.general.file_ex_filter);
	}else{
		bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
		tstring list_file_path;
		File list_file;

		if(use_filter){
			//リストファイルを作成
			list_file_path=tempfile::create(_T("rar"),ARCCFG->m_list_temp_dir.c_str());
			//BOMなしファイルを出力
			if(list_file.open(list_file_path.c_str(),
							  OPEN_ALWAYS,
							  GENERIC_WRITE,
							  0,
							  (isUnicodeMode())?static_cast<File::CODEPAGE>(File::UTF8|File::NO_BOM):File::SJIS)){
				//リストファイルに列挙対象ファイルのみ出力
				outputFileListEx(arc_path_str.c_str(),
								 CFG.general.filefilter,
								 CFG.general.file_ex_filter,
								 0,
								 &list_file);
				list_file.close();
			}else{
				use_filter=false;
			}
		}

		tstring cmd_line(format(_T("%s %s %s %s"),
										  _T("-l"),
										  _T("-q"),
										  _T("--"),
										  path::quote(arc_path_str).c_str()));

		if(use_filter){
			cmd_line.append(format(_T(" @%s"),
								   path::quote(list_file_path).c_str()));
		}

		dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

		tstring log_msg;

		execute(NULL,cmd_line.c_str(),&log_msg,log_buffer_size);
		STDOUT.outputStringF(Console::LOW_GREEN,Console::NONE,_T("%s\n"),log_msg.c_str());
	}
	return ARC_SUCCESS;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcUnrar32::writeFormatedPath(const File& list_file,const TCHAR* base_dir,const TCHAR* file_path){
	return -1;
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcUnrar32::writeFormatedList(const File& list_file,const tstring& full_path){
	return false;
}

//処理を中止する
void ArcUnrar32::abort(){
	//CANCEL押下
	for(;::IsWindow(hook::extracting_wnd_handle);){
		::SendDlgItemMessage(hook::extracting_wnd_handle,IDCANCEL,BM_CLICK,0,0);
		::Sleep(100);
	}
	terminateApp();
}

//ArcDll::callbackProcV()に投げるための準備
bool ArcUnrar32::preCallback(const TCHAR* file_name,const TCHAR* progress){
	if(CFG.no_display.no_information||STDOUT.isRedirected())return false;

	EXTRACTINGINFOEX64 arc_info={};

	if(file_name)m_processing_info.file_name=file_name;
	m_processing_info.done=++hook::progress_done;
	m_processing_info.total=hook::progress_total;

	if(!callbackProcV(NULL,0,/*ARCEXTRACT_INPROCESS,*/&arc_info)){
		return false;
	}
	return true;
}

//ファイル処理情報を格納
void ArcUnrar32::setExtractingInfo(/*UINT state,*/void* arc_info){
	//dwFileSizeは処理中のファイルのサイズ
	//dwWriteSizeは処理中ファイルの書き込まれたサイズ
	switch(m_extracting_info_struct_size){
		case sizeof(EXTRACTINGINFOEX64):
			break;

		case sizeof(EXTRACTINGINFOEX):{
#if 0
			//処理中ファイル名
			m_processing_info.file_name=(isUnicodeMode())?
				str::utf82utf16(((LPEXTRACTINGINFOEX)arc_info)->exinfo.szSourceFileName):
				str::sjis2utf16(((LPEXTRACTINGINFOEX)arc_info)->exinfo.szSourceFileName);

			//ファイルサイズ
			m_processing_info.total=m_file_size;

			//処理済みサイズ
			DWORD write_size=0;

			if(((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwFileSize==0){
				m_processing_info.done=m_write_size;
				break;
			}

			if(((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize==0){
				//初めてそのファイルの処理を行う場合
				write_size=((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize;
				m_write_size++;
			}else{
				//前回から継続して同じファイルの処理する場合
				write_size=(m_last_write_size>((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize)?
					m_last_write_size-((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize:
					((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize-m_last_write_size;
			}

			m_last_write_size=((LPEXTRACTINGINFOEX)arc_info)->exinfo.dwWriteSize;

			m_processing_info.done=
				m_write_size+=write_size;
#endif
			break;
		}
	}
}

//関数対応を操作
bool ArcUnrar32::querySupport(int func){
	switch(func){
		case ISARC_SETOWNERWINDOW:
		case ISARC_CLEAROWNERWINDOW:
		case ISARC_SETOWNERWINDOWEX:
		case ISARC_KILLOWNERWINDOWEX:
			return false;

		default:
			break;
	}
	return true;
}
