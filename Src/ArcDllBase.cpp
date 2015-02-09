//ArcDllBase.cpp
//統合アーカイバDll操作クラス
//一部の関数のみに対応

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r25 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcDllBase.h"

using namespace sslib;



ArcDllBase::ArcDllBase():m_arc_handle(NULL),m_unicode_mode(false){
}

ArcDllBase::ArcDllBase(const TCHAR* library_name,const TCHAR* library_prefix):
	Archiver(library_name,library_prefix),
	m_arc_handle(NULL),
	m_unicode_mode(false){
}

ArcDllBase::~ArcDllBase(){
	closeArchive();
}

//ライブラリを読み込む
bool ArcDllBase::load(){
	m_unicode_mode=false;
	m_arc_handle=NULL;

	if(Archiver::load()){
#ifdef UNICODE
		setUnicodeMode(true);
#endif
		return true;
	}
	return false;
}

//ライブラリを読み込む
bool ArcDllBase::load(const TCHAR* library_name,const TCHAR* library_prefix){
	m_unicode_mode=false;
	m_arc_handle=NULL;

	if(Archiver::load(library_name,library_prefix)){
#ifdef UNICODE
		setUnicodeMode(true);
#endif
		return true;
	}
	return false;
}

//ライブラリを解放
bool ArcDllBase::unload(){
	if(isLoaded())closeArchive();
	return Archiver::unload();
}

//関数対応を操作
bool ArcDllBase::querySupport(int func){return true;}

//引数を渡して実行
int ArcDllBase::execute(const HWND wnd_handle,const TCHAR* cmdline,tstring* result_buffer,const DWORD buffer_size){
	typedef int (WINAPI*EXECUTE_PTR)(const HWND,const char*,char*,const DWORD);
	EXECUTE_PTR p_execute;
	int result=-1;

	if((p_execute=(EXECUTE_PTR)getAddress(_T("")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_execute(wnd_handle,
						 ((isUnicodeMode())?str::utf162utf8(cmdline):str::utf162sjis(cmdline)).c_str(),
						 &buffer_multibyte[0],
						 buffer_size);

		if(result_buffer){
			if(isUnicodeMode()){
				str::utf82utf16(result_buffer,&buffer_multibyte[0]);
			}else{
				str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
			}
		}
	}
	return result;
}

//DLLのバージョンを返します(現在のバージョンの上位2ワードに100を掛けたものが返ってきます)
bool ArcDllBase::getVersion(WORD* version){
	typedef WORD(WINAPI*GETVERSION_PTR)();
	GETVERSION_PTR p_getVersion;
	bool result=false;

	if(queryFunctionList(ISARC_GET_VERSION)){
		if((p_getVersion=(GETVERSION_PTR)getAddress(_T("GetVersion")))!=NULL){
			*version=p_getVersion();
			result=true;
		}
	}
	return result;
}

//DLLのバージョンを返します(現在のバージョンの下位2ワードに100を掛けたものが返ってきます)
bool ArcDllBase::getSubVersion(WORD* version){
	typedef WORD(WINAPI*GETSUBVERSION_PTR)();
	GETSUBVERSION_PTR p_getSubVersion;
	bool result=false;

	if((p_getSubVersion=(GETSUBVERSION_PTR)getAddress(_T("GetSubVersion")))!=NULL){
		*version=p_getSubVersion();
		result=true;
	}
	return result;
}

//DLLの動作中にカーソルを表示するモードか否かを設定します
bool ArcDllBase::setCursorMode(const bool cursor_mode){
	typedef BOOL(WINAPI*SETCURSORMODE_PTR)(const BOOL);
	SETCURSORMODE_PTR p_setCursorMode;
	bool result=false;

	if(queryFunctionList(ISARC_SET_CURSOR_MODE)){
		if((p_setCursorMode=(SETCURSORMODE_PTR)getAddress(_T("SetCursorMode")))!=NULL){
			result=p_setCursorMode(cursor_mode)!=0;
		}
	}
	return result;
}

//現在DLLが動作中か否かを得ます
bool ArcDllBase::getRunning(){
	typedef BOOL(WINAPI*GETRUNNING_PTR)();
	GETRUNNING_PTR p_getRunning;
	bool result=false;

	if(queryFunctionList(ISARC_GET_RUNNING)){
		if((p_getRunning=(GETRUNNING_PTR)getAddress(_T("GetRunning")))!=NULL){
			result=p_getRunning()!=0;
		}
	}
	return result;
}

#if 0
//DLLがバックグラウンドモードか否かを得ます
bool ArcDllBase::getBackGroundMode(){
	typedef BOOL(WINAPI*GETBACKGROUNDMODE_PTR)();
	GETBACKGROUNDMODE_PTR p_getBackGroundMode;
	bool result=false;

	if(queryFunctionList(ISARC_GET_BACK_GROUND_MODE)){
		if((p_getBackGroundMode=(GETBACKGROUNDMODE_PTR)getAddress(_T("GetBackGroundMode")))!=NULL){
			result=p_getBackGroundMode()!=0;
		}
	}
	return result;
}

//DLLのバックグラウンドモードを設定します
bool ArcDllBase::setBackGroundMode(const bool back_ground_mode){
	typedef BOOL(WINAPI*SETBACKGROUNDMODE_PTR)(BOOL);
	SETBACKGROUNDMODE_PTR p_setBackGroundMode;
	bool result=false;

	if(queryFunctionList(ISARC_SET_BACK_GROUND_MODE)){
		if((p_setBackGroundMode=(SETBACKGROUNDMODE_PTR)getAddress(_T("SetBackGroundMode")))!=NULL){
			result=p_setBackGroundMode(back_ground_mode)!=0;
		}
	}
	return result;
}
#endif

//指定ファイルがサポートしている書庫ファイルとして
//正しいかどうかを返します
bool ArcDllBase::checkArchive(const TCHAR* file_name,const int mode){
	typedef BOOL(WINAPI*CHECKARCHIVE_PTR)(const char*,const int);
	CHECKARCHIVE_PTR p_checkArchive;
	bool result=false;

	if(queryFunctionList(ISARC_CHECK_ARCHIVE)){
		if((p_checkArchive=(CHECKARCHIVE_PTR)getAddress(_T("CheckArchive")))!=NULL){
			result=p_checkArchive((isUnicodeMode()?str::utf162utf8(file_name):str::utf162sjis(file_name)).c_str(),mode)!=0;
		}
	}
	return result;
}

//指定したアーカイブファイルの形式を得ます
int ArcDllBase::getArchiveType(const TCHAR* file_name){
	typedef int(WINAPI*GETARCHIVETYPE_PTR)(const char*);
	GETARCHIVETYPE_PTR p_getArchiveType;
	int result=-1;

	if((p_getArchiveType=(GETARCHIVETYPE_PTR)getAddress(_T("GetArchiveType")))!=NULL){
		result=p_getArchiveType((isUnicodeMode()?str::utf162utf8(file_name):str::utf162sjis(file_name)).c_str());
	}
	return result;
}

//指定されたAPIが使用可能かどうかを得ます
bool ArcDllBase::queryFunctionList(const int function){
	typedef BOOL(WINAPI*QUERYFUNCTIONLIST_PTR)(const int);
	QUERYFUNCTIONLIST_PTR p_queryFunctionList;
	bool result=false;

	if((p_queryFunctionList=(QUERYFUNCTIONLIST_PTR)getAddress(_T("QueryFunctionList")))!=NULL){
		result=
			querySupport(function)&&
			//ライブラリ側
			p_queryFunctionList(function)!=0;
	}
	return result;
}

//オプション指定ダイアログの表示
bool ArcDllBase::configDialog(HWND wnd_handle,tstring* result_buffer,const int mode){
	typedef BOOL(WINAPI*CONFIGDIALOG_PTR)(HWND,char*,const int);
	CONFIGDIALOG_PTR p_configDialog;
	bool result=false;

	if(queryFunctionList(ISARC_CONFIG_DIALOG)){
		if((p_configDialog=(CONFIGDIALOG_PTR)getAddress(_T("ConfigDialog")))!=NULL){
			//API.TXT(unlha32.dll)より
			//レジストリーに書き込まれる内容と同様の動作を行う為
			//のコマンド文字列を得るためのバッファのポインタ。用
			//意するバッファは 513 バイト (Unicode 版では 513 文
			//字) 以上を確保してください。
			std::vector<char> buffer_multibyte(513);

			result=p_configDialog(wnd_handle,&buffer_multibyte[0],mode)!=0;

			if(result_buffer!=NULL){
				if(isUnicodeMode()){
					str::utf82utf16(result_buffer,&buffer_multibyte[0]);
				}else{
					str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
				}
			}
		}
	}
	return result;
}

//指定ファイルが有効な書庫ファイルかどうか調べ、
//有効であればオープンし、一意の数値(ハンドル)を返します
HARC ArcDllBase::openArchive(const HWND wnd_handle,const TCHAR* file_name,const DWORD mode){
	typedef HARC(WINAPI*OPENARCHIVE_PTR)(const HWND,const char*,const DWORD);
	OPENARCHIVE_PTR p_openArchive;

	closeArchive();
	if(queryFunctionList(ISARC_OPEN_ARCHIVE)){
		if((p_openArchive=(OPENARCHIVE_PTR)getAddress(_T("OpenArchive")))!=NULL){
			if(getRunning()==false){
				m_arc_handle=p_openArchive(wnd_handle,
										   (isUnicodeMode()?str::utf162utf8(file_name):str::utf162sjis(file_name)).c_str(),
										   mode);
			}
		}
	}
	return m_arc_handle;
}

//OpenArchive()で割り付けたハンドルを解放する
bool ArcDllBase::closeArchive(){
	typedef int(WINAPI*CLOSEARCHIVE_PTR)(HARC);
	CLOSEARCHIVE_PTR p_closeArchive;
	bool result=false;

	if(!isOpened())return result;

	if(queryFunctionList(ISARC_CLOSE_ARCHIVE)){
		if((p_closeArchive=(CLOSEARCHIVE_PTR)getAddress(_T("CloseArchive")))!=NULL){
			result=p_closeArchive(handle())==0;
		}
	}
	m_arc_handle=NULL;
	return result;
}

//最初の格納ファイルの情報を得ます
int ArcDllBase::findFirst(const TCHAR* wild_name,LPINDIVIDUALINFO info){
	typedef int(WINAPI*FINDFIRST_PTR)(HARC,const char*,LPINDIVIDUALINFO);
	FINDFIRST_PTR p_findFirst;
	int result=-1;

	if(queryFunctionList(ISARC_FIND_FIRST)){
		if((p_findFirst=(FINDFIRST_PTR)getAddress(_T("FindFirst")))!=NULL){
			//XacRettにNULLを渡すと落ちてしまう
			result=p_findFirst(handle(),
							   (isUnicodeMode()?str::utf162utf8(wild_name):str::utf162sjis(wild_name)).c_str(),
							   info);
		}
	}
	return result;
}

//次の格納ファイルの情報を得ます
int ArcDllBase::findNext(LPINDIVIDUALINFO info){
	typedef int(WINAPI*FINDNEXT_PTR)(HARC,LPINDIVIDUALINFO FAR);
	FINDNEXT_PTR p_findNext;
	int result=-1;

	if(queryFunctionList(ISARC_FIND_NEXT)){
		if((p_findNext=(FINDNEXT_PTR)getAddress(_T("FindNext")))!=NULL){
			//XacRettにNULLを渡すと落ちてしまう
			result=p_findNext(handle(),info);
		}
	}
	return result;
}

//DLLのメッセージの送信先ウィンドウを設定します
bool ArcDllBase::setOwnerWindow(HWND wnd_handle){
	typedef int(WINAPI*SETOWNERWINDOW_PTR)(HWND);
	SETOWNERWINDOW_PTR p_setOwnerWindow;
	bool result=false;

	if(queryFunctionList(ISARC_SETOWNERWINDOW)){
		if((p_setOwnerWindow=(SETOWNERWINDOW_PTR)getAddress(_T("SetOwnerWindow")))!=NULL){
			result=p_setOwnerWindow(wnd_handle)!=0;
		}
	}
	return result;
}

//SetOwnerWindow()で設定したウィンドウの設定を解除します
bool ArcDllBase::clearOwnerWindow(){
	typedef int(WINAPI*CLEAROWNERWINDOW_PTR)();
	CLEAROWNERWINDOW_PTR p_clearOwnerWindow;
	bool result=false;

	if(queryFunctionList(ISARC_CLEAROWNERWINDOW)){
		if((p_clearOwnerWindow=(CLEAROWNERWINDOW_PTR)getAddress(_T("ClearOwnerWindow")))!=NULL){
			result=p_clearOwnerWindow()!=0;
		}
	}
	return result;
}

//DLLの圧縮解凍状況を受け取るためのコールバック関数の指定を行います
bool ArcDllBase::setOwnerWindowEx(HWND wnd_handle,LPARCHIVERPROC arc_proc){
	typedef BOOL(WINAPI*SETOWNERWINDOWEX_PTR)(HWND,LPARCHIVERPROC);
	SETOWNERWINDOWEX_PTR p_setOwnerWindowEx;
	bool result=false;

	if(queryFunctionList(ISARC_SETOWNERWINDOWEX)){
		if((p_setOwnerWindowEx=(SETOWNERWINDOWEX_PTR)getAddress(_T("SetOwnerWindowEx")))!=NULL){
			result=p_setOwnerWindowEx(wnd_handle,arc_proc)!=0;
		}
	}
	return result;
}
//SetOwnerWindowEx()で設定したウィンドウの設定を解除します
bool ArcDllBase::killOwnerWindowEx(HWND wnd_handle){
	typedef BOOL(WINAPI*KILLOWNERWINDOWEX_PTR)(HWND);
	KILLOWNERWINDOWEX_PTR p_killOwnerWindowEx;
	bool result=false;

	if(queryFunctionList(ISARC_KILLOWNERWINDOWEX)){
		if((p_killOwnerWindowEx=(KILLOWNERWINDOWEX_PTR)getAddress(_T("KillOwnerWindowEx")))!=NULL){
			result=p_killOwnerWindowEx(wnd_handle)!=0;
		}
	}
	return result;
}

//格納ファイルのファイル名を得ます
int ArcDllBase::getFileName(tstring* result_buffer,const int buffer_size){
	typedef int(WINAPI*GETFILENAME_PTR)(HARC,char*,const int);
	GETFILENAME_PTR p_getFileName;
	int result=-1;

	if(queryFunctionList(ISARC_GET_FILE_NAME)){
		if((p_getFileName=(GETFILENAME_PTR)getAddress(_T("GetFileName")))!=NULL){
			std::vector<char> buffer_multibyte(buffer_size);

			result=p_getFileName(handle(),&buffer_multibyte[0],buffer_size);

			if(isUnicodeMode()){
				str::utf82utf16(result_buffer,&buffer_multibyte[0]);
			}else{
				str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
			}
		}
	}
	return result;
}

//格納ファイルのサイズを64ビット整数で得ます
bool ArcDllBase::getOriginalSizeEx(__int64* size){
	typedef BOOL(WINAPI*GETORIGINALSIZEEX_PTR)(HARC,__int64*);
	GETORIGINALSIZEEX_PTR p_getOriginalSizeEx;
	bool result=false;

	if(queryFunctionList(ISARC_GET_COMPRESSED_SIZE)){
		if((p_getOriginalSizeEx=(GETORIGINALSIZEEX_PTR)getAddress(_T("GetOriginalSizeEx")))!=NULL){
			result=p_getOriginalSizeEx(handle(),size)!=0;
		}
	}
	return result;
}

//格納ファイルの圧縮サイズを得ます
bool ArcDllBase::getCompressedSizeEx(__int64* size){
	typedef BOOL(WINAPI*GETCOMPRESSEDSIZEEX_PTR)(HARC,__int64*);
	GETCOMPRESSEDSIZEEX_PTR p_getCompressedSizeEx;
	bool result=false;

	if(queryFunctionList(ISARC_GET_COMPRESSED_SIZE_EX)){
		if((p_getCompressedSizeEx=(GETCOMPRESSEDSIZEEX_PTR)getAddress(_T("GetCompressedSizeEx")))!=NULL){
			result=p_getCompressedSizeEx(handle(),size)!=0;
		}
	}
	return result;
}

//格納ファイルの属性を得ます
bool ArcDllBase::getAttribute(DWORD* p_attr,int* p_orig_attr){
	typedef int(WINAPI*GETATTRIBUTE_PTR)(HARC);
	GETATTRIBUTE_PTR p_getAttribute;
	bool result=false;

	if(queryFunctionList(ISARC_GET_ATTRIBUTE)){
		if((p_getAttribute=(GETATTRIBUTE_PTR)getAddress(_T("GetAttribute")))!=NULL){
			int attr=p_getAttribute(handle());
			if(attr==-1)return result;

			if(p_orig_attr)*p_orig_attr=attr;
			if(!p_attr){
				result=true;
				return result;
			}

			if(attr&FA_RDONLY)*p_attr|=FILE_ATTRIBUTE_READONLY;
			if(attr&FA_HIDDEN)*p_attr|=FILE_ATTRIBUTE_HIDDEN;
			if(attr&FA_SYSTEM)*p_attr|=FILE_ATTRIBUTE_SYSTEM;
//			if(attr&FA_LABEL)
			if(attr&FA_DIREC)*p_attr|=FILE_ATTRIBUTE_DIRECTORY;
//			if(attr&FA_ARCH)
//			if(attr&FA_ENCRYPTED)
			result=true;
		}
	}
	return result;
}

//書庫の情報を得る際に使用する標準パスワードを設定します。
int ArcDllBase::setDefaultPassword(const TCHAR*password){
	typedef int(WINAPI*SETDEFAULTPASSWORD_PTR)(HARC,const char*);
	SETDEFAULTPASSWORD_PTR p_setDefaultPassword;
	int result=0;

	if((p_setDefaultPassword=(SETDEFAULTPASSWORD_PTR)getAddress(_T("SetDefaultPassword")))!=NULL){
		result=p_setDefaultPassword(handle(),
									(password)?(isUnicodeMode()?str::utf162utf8(password):str::utf162sjis(password)).c_str():NULL);
	}
	return result;
}

//使用する文字コードをUTF-8に設定、及び解除を行います。(7-zip32)
//ANSI 版 API を UTF-8 の文字コードで使用するかどうかを指定します。(UNLHA32.DLL)
bool ArcDllBase::setUnicodeMode(bool unicode){
	typedef BOOL(WINAPI*SETUNICODEMODE_PTR)(BOOL);
	SETUNICODEMODE_PTR p_setUnicodeMode;
	bool result=false;

	if(queryFunctionList(ISARC_SET_UNICODE_MODE)){
		if((p_setUnicodeMode=(SETUNICODEMODE_PTR)getAddress(_T("SetUnicodeMode")))!=NULL){
			result=p_setUnicodeMode(unicode)!=0;
		}
	}
	m_unicode_mode=result;
	return result;
}

//DLLの圧縮解凍状況を受け取るためのコールバック関数の指定を行います
bool ArcDllBase::setOwnerWindowEx64(const HWND wnd_handle,LPARCHIVERPROC arc_proc,const DWORD struct_size){
	typedef BOOL(WINAPI*SETOWNERWINDOWEX64_PTR)(HWND,LPARCHIVERPROC,const DWORD);
	SETOWNERWINDOWEX64_PTR p_setOwnerWindowEx64;
	bool result=false;

	if(queryFunctionList(ISARC_SETOWNERWINDOWEX64)){
		if((p_setOwnerWindowEx64=(SETOWNERWINDOWEX64_PTR)getAddress(_T("SetOwnerWindowEx64")))!=NULL){
			result=p_setOwnerWindowEx64(wnd_handle,arc_proc,struct_size)!=0;
		}
	}
	return result;
}

//SetOwnerWindowEx64()で設定したウィンドウの設定を解除します
bool ArcDllBase::killOwnerWindowEx64(HWND wnd_handle){
	typedef BOOL(WINAPI*KILLOWNERWINDOWEX64_PTR)(HWND);
	KILLOWNERWINDOWEX64_PTR p_killOwnerWindowEx64;
	bool result=false;

	if(queryFunctionList(ISARC_KILLOWNERWINDOWEX64)){
		if((p_killOwnerWindowEx64=(KILLOWNERWINDOWEX64_PTR)getAddress(_T("KillOwnerWindowEx64")))!=NULL){
			result=p_killOwnerWindowEx64(wnd_handle)!=0;
		}
	}
	return result;
}
