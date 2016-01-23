//ArcXacrett.cpp
//Xacrett.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r31 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcXacrett.h"
#include"ArcCfg.h"

using namespace sslib;

namespace{
namespace hook{
	typedef void (WINAPI*pGetTextExtentPointA)(HDC hdc,LPCSTR lpString,int cbString,LPSIZE lpSize);
	typedef HWND (WINAPI*pGetDlgItem)(HWND hDlg,int nIDDlgItem);
	typedef BOOL (WINAPI*pSetWindowTextA)(HWND hWnd,LPCSTR lpString);

	//ArcXacrettのコンストラクタで代入すること
	ArcXacrett* this_ptr=NULL;
	//IDC_ENAME
	HWND ename_wnd=NULL;

	pGetTextExtentPointA org_pGetTextExtentPointA;
	pGetDlgItem org_pGetDlgItem;
	pSetWindowTextA org_pSetWindowTextA;

	void WINAPI getTextExtentPointA(HDC hdc,LPCSTR lpString,int cbString,LPSIZE lpSize){
		org_pGetTextExtentPointA(hdc,lpString,cbString,lpSize);
		//PathSetDlgItemPath()によるパスの省略を防止。
		lpSize->cx=0;
		return;
	}

	HWND WINAPI getDlgItem(HWND hDlg,int nIDDlgItem){
		HWND result=org_pGetDlgItem(hDlg,nIDDlgItem);

		if(nIDDlgItem==1004){
			//IDC_ENAMEならハンドルを保存
			ename_wnd=result;
		}
		return result;
	}

	BOOL WINAPI setWindowTextA(HWND hWnd,LPCSTR lpString){
		if(ename_wnd&&ename_wnd==hWnd){
			if(this_ptr)this_ptr->preCallback(lpString);
		}
		return org_pSetWindowTextA(hWnd,lpString);
	}

	void init(){
		//GetTextExtentPointA
		org_pGetTextExtentPointA=(pGetTextExtentPointA)GetProcAddress(GetModuleHandleA("gdi32"),"GetTextExtentPointA");
		//GetDlgItem
		org_pGetDlgItem=(pGetDlgItem)GetProcAddress(GetModuleHandleA("user32"),"GetDlgItem");
		//SetWindowTextA
		org_pSetWindowTextA=(pSetWindowTextA)GetProcAddress(GetModuleHandleA("user32"),"SetWindowTextA");
	}

	void install(){
		init();
		//GetTextExtentPointA
		apihook::replace("gdi32.dll",
						 reinterpret_cast<PROC>(org_pGetTextExtentPointA),
						 reinterpret_cast<PROC>(getTextExtentPointA),
						 ::GetModuleHandleA("shlwapi.dll"));
		//GetDlgItem
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pGetDlgItem),
						 reinterpret_cast<PROC>(getDlgItem),
						 ::GetModuleHandleA("shlwapi.dll"));
		//SetWindowTextA
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(org_pSetWindowTextA),
						 reinterpret_cast<PROC>(setWindowTextA),
						 ::GetModuleHandleA("shlwapi.dll"));
	}

	void uninstall(){
		//GetTextExtentPointA
		apihook::replace("gdi32.dll",
						 reinterpret_cast<PROC>(getTextExtentPointA),
						 reinterpret_cast<PROC>(org_pGetTextExtentPointA),
						 ::GetModuleHandleA("shlwapi.dll"));
		//GetDlgItem
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(getDlgItem),
						 reinterpret_cast<PROC>(org_pGetDlgItem),
						 ::GetModuleHandleA("shlwapi.dll"));
		//SetWindowTextA
		apihook::replace("user32.dll",
						 reinterpret_cast<PROC>(setWindowTextA),
						 reinterpret_cast<PROC>(org_pSetWindowTextA),
						 ::GetModuleHandleA("shlwapi.dll"));
	}
}
}

ArcXacrett::ArcXacrett():
	ArcDll(_T("XacRett"),
			_T("XacRett"),
			_T("ace.arc.arg.arj.asd.b2a.b64.bel.bin.boo.bza.gza.C.cab.cpt.dmg.dz.F.xF.gca.hqx.imp.ish.jak.ntx.pit.pak.wad.pff.rez.sp.spl.uue.xxe.zac.zoo"),
			_T("\\")),
	m_log_msg(NULL),
	m_file_size(0),
	m_write_size(0){
		COMPRESSION_FORMAT format[]={
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_formats.assign(format,format+ARRAY_SIZEOF(format));
		hook::this_ptr=this;
}

ArcXacrett::ARC_RESULT ArcXacrett::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	return ARC_NOT_IMPLEMENTED;
}

ArcXacrett::ARC_RESULT ArcXacrett::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	tstring arc_path_str(arc_path);
	tstring output_dir_str(output_dir);

	m_file_size=m_write_size=0;
	m_log_msg=log_msg;
	if(m_log_msg)m_log_msg->append(_T("\n"));

	bool use_filter=!CFG.general.filefilter.empty()||!CFG.general.file_ex_filter.empty();
	tstring list_file_path;
	File list_file;

	if(use_filter){
		//リストファイルを作成
		list_file_path=tempfile::create(_T("xcr"),ARCCFG->m_list_temp_dir.c_str());
		if(!list_file.open(list_file_path.c_str(),OPEN_ALWAYS,GENERIC_WRITE,0,(isUnicodeMode())?File::UTF8:File::SJIS)){
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
		m_extracting_info_struct_size=sizeof(EXTRACTINGINFOEX);
		//プログレスバー表示の為にリスト作成の必要あり
		m_file_size=getTotalOriginalSize(arc_path);

		//IAT書き換え
		hook::install();
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
							(!CFG.general.ignore_directory_structures)?_T("-x"):_T("-x -j"),
							_T("-o1"),
							CFG.general.custom_param.c_str(),
							//-o1    :上書き確認ダイアログ表示の抑止
							//			_T("-n1"),
							_T(""),

							path::quote(arc_path_str).c_str(),

							path::quote(output_dir_str).c_str()));

	if(use_filter){
		//注意:'@'も二重引用府内に含めること
		cmd_line.append(format(_T(" %s"),
							   path::quote(_T("@")+list_file_path).c_str()));
	}

	dprintf(_T("%s:%s\n"),name().c_str(),cmd_line.c_str());

	msg::info(_T("'%s'を解凍しています...\n\n"),arc_path);

	bool use_password=!CFG.general.password_list.empty();

	std::list<tstring>::iterator ite_password_list=CFG.general.password_list.begin();
	std::list<tstring>::iterator password_list_end=CFG.general.password_list.end();

	int dll_ret=-1;
	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		m_processing_info.clear();
	}

	do{
		if(use_password){
			CFG.general.password=*ite_password_list;
		}

		//実行
		//ログは自前で作成
		//(setExtractingInfo()でファイル名を追加していくだけ...)
//		if(CFG.no_display.no_log||log_msg==NULL){
			tstring dummy(1,'\0');

			dll_ret=execute(NULL,cmd_line.c_str(),&dummy,dummy.length());
//		}else{
//			dll_ret=execute(NULL,cmd_line.get(),log_msg,log_buffer_size);
//		}

	}while(use_password&&
		   dll_ret!=0&&
		   (++ite_password_list)!=password_list_end);

	if(!CFG.no_display.no_information&&
	   !STDOUT.isRedirected()){
		hook::uninstall();
	}

	msg::info(_T("\n   => return code %d[%#x]\n"),dll_ret,dll_ret);


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

ArcXacrett::ARC_RESULT ArcXacrett::list(const TCHAR* arc_path){
	tstring arc_path_str(arc_path);

	replaceDelimiter(arc_path_str);

	//現バージョンでは、リスト表示コマンドが実装されていないため、
	//reces側で情報を表示
	if(!openArchive(NULL,arc_path_str.c_str(),0))return ARC_FAILURE;

	STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("\n   Date      Time   Attr         Size Name\n"));
	STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("---------- -------- ---- ------------ ------------------------\n"));

	ArcFileSearch fs(this);

	if(fs.first()){
		do{
			fileinfo::FILEINFO* fileinfo=fs.getFileInfo();
			SYSTEMTIME st={0};

			if(!fileinfo::matchFilters(*fileinfo,CFG.general.filefilter,CFG.general.file_ex_filter))continue;

			strex::longlong2SYSTEMTIME(&st,fileinfo->date_time);

			STDOUT.outputString(Console::LOW_GREEN,Console::NONE,_T("%04u/%02u/%02u %02u:%02u:%02u %s%s%s%s %12I64d %s\n"),
										 st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,
										 (fileinfo->attr&FILE_ATTRIBUTE_DIRECTORY)?_T("D"):_T("."),
										 (fileinfo->attr&FILE_ATTRIBUTE_READONLY)?_T("R"):_T("."),
										 (fileinfo->attr&FILE_ATTRIBUTE_HIDDEN)?_T("H"):_T("."),
										 (fileinfo->attr&FILE_ATTRIBUTE_SYSTEM)?_T("S"):_T("."),
										 fileinfo->size,
										 fileinfo->name.c_str());
		}while(!IS_TERMINATED&&fs.next());
	}
	closeArchive();
	return ARC_SUCCESS;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcXacrett::writeFormatedPath(const File& list_file,const TCHAR* base_dir,const TCHAR* file_path){
	return -1;
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcXacrett::writeFormatedList(const File& list_file,const tstring& full_path){
	return false;
}

//処理を中止する
void ArcXacrett::abort(){
	//CANCEL押下
	for(;::IsWindow(ARCCFG->m_extracting_wnd_handle);){
		::SendDlgItemMessage(ARCCFG->m_extracting_wnd_handle,IDCANCEL,BM_CLICK,0,0);
		::Sleep(100);
	}
	terminateApp();
}

//ArcDll::callbackProcV()に投げるための準備
bool ArcXacrett::preCallback(const char* file_name){
	EXTRACTINGINFOEX arc_info={};

	//処理済みサイズ(正しい値はsetExtractingInfo()で)
	arc_info.exinfo.dwWriteSize=1;
	//ファイルサイズ(正しい値はsetExtractingInfo()で)
	arc_info.exinfo.dwFileSize=1;
	//処理中ファイル名
	lstrcpynA(arc_info.exinfo.szSourceFileName,file_name,FNAME_MAX32);

	if(!callbackProcV(NULL,0,/*ARCEXTRACT_INPROCESS,*/&arc_info)){
		hook::uninstall();
		return false;
	}
	return true;
}

//ファイル処理情報を格納
void ArcXacrett::setExtractingInfo(/*UINT state,*/void* arc_info){
	switch(m_extracting_info_struct_size){
		case sizeof(EXTRACTINGINFOEX64):
			break;

		case sizeof(EXTRACTINGINFOEX):{
			//処理中ファイル名
			if(isUnicodeMode()){
				str::utf82utf16(&m_processing_info.file_name,((LPEXTRACTINGINFOEX)arc_info)->exinfo.szSourceFileName);
			}else{
				str::sjis2utf16(&m_processing_info.file_name,((LPEXTRACTINGINFOEX)arc_info)->exinfo.szSourceFileName);
			}

			//ファイルサイズ
			m_processing_info.total=m_file_size;

			//処理済みサイズ
			bool found=false;

			for(std::vector<fileinfo::FILEINFO>::const_iterator ite=m_arc_info.file_list.begin(),
				end=m_arc_info.file_list.end();
				ite!=end;
				++ite){
				if(ite->name==m_processing_info.file_name){
					m_processing_info.done=m_write_size+=ite->size;
					found=true;
					break;
				}
			}
			if(!found)m_processing_info.file_name.clear();
			if(m_log_msg&&
			   !m_processing_info.file_name.empty()){
				m_log_msg->append(m_processing_info.file_name+_T("\n"));
			}
			break;
		}
	}
}
