//SpiBase.cpp
//Spi操作クラス
//一部の関数のみに対応(書庫関連)

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r34 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"StdAfx.h"
#include"SpiBase.h"

using namespace sslib;


//Plug-inに関する情報を得る
int SpiBase::getPluginInfo(int info_no,tstring* result_buffer,int buffer_length){
	typedef	int (WINAPI*SPI_GETPLUGININFO)(int,LPCSTR,int);
	SPI_GETPLUGININFO GetPluginInfo;
	int result=0;

	if((GetPluginInfo=(SPI_GETPLUGININFO)getAddress(_T("GetPluginInfo")))!=NULL){
		std::vector<char> buffer(buffer_length);

		result=GetPluginInfo(info_no,&buffer[0],buffer_length);
		str::sjis2utf16(result_buffer,std::string(&buffer[0]));
	}
	return result;
}

//展開可能な(対応している)ファイル形式か調べる。
int SpiBase::isSupported(const TCHAR* file_path,void* param){
	typedef int(WINAPI*SPI_ISSUPPORTED)(LPCSTR,void*);
	SPI_ISSUPPORTED IsSupported;
	int result=0;

	if((IsSupported=(SPI_ISSUPPORTED)getAddress(_T("IsSupported")))!=NULL){
		result=IsSupported(str::utf162sjis(file_path).c_str(),param);
	}
	return result;
}

//アーカイブ内のすべてのファイルの情報を取得する(ファイル入力)
SpiBase::SPI_ERROR SpiBase::getArchiveInfo(const TCHAR* file_path,long buffer_length,unsigned int flag,HLOCAL* info){
	typedef int(WINAPI*SPI_GETARCHIVEINFO)(LPCSTR,long,unsigned int,HLOCAL*);
	SPI_GETARCHIVEINFO GetArchiveInfo;
	SPI_ERROR result=SPI_ERROR_NOT_IMPLEMENT;

	if((GetArchiveInfo=(SPI_GETARCHIVEINFO)getAddress(_T("GetArchiveInfo")))!=NULL){
		result=static_cast<SPI_ERROR>(GetArchiveInfo(str::utf162sjis(file_path).c_str(),buffer_length,flag,info));
	}
	return result;
}

#if 0
//アーカイブ内の指定したファイルの情報を取得する
SpiBase::SPI_ERROR SpiBase::getFileInfo(const TCHAR* file_path,long buffer_length,char* dir_path,unsigned int flag,fileInfo* file_info){
	typedef int(WINAPI*SPI_GETFILEINFO)(LPCSTR,long,LPSTR,unsigned int,fileInfo*);
	SPI_GETFILEINFO GetFileInfo;
	SPI_ERROR result=SPI_ERROR_NOT_IMPLEMENT;

	if((GetFileInfo=(SPI_GETFILEINFO)getAddress(_T("GetFileInfo")))!=NULL){
		result=static_cast<SPI_ERROR>(GetFileInfo(str::utf162sjis(file_path).c_str(),buffer_length,dir_path,flag,file_info));
	}
	return result;
}
#endif

//アーカイブ内のファイルを取得する
//注意:ファイル入力/メモリ出力のみ動作確認
SpiBase::SPI_ERROR SpiBase::getFile(const TCHAR* file_path,long buffer_length,void* dest,unsigned int flag,SPI_CALLBACK callback,long data){
	typedef int(WINAPI*SPI_GETFILE)(LPCSTR,long,void*,unsigned int,SPI_CALLBACK,long);
	SPI_GETFILE GetFile;
	SPI_ERROR result=SPI_ERROR_NOT_IMPLEMENT;

	if((GetFile=(SPI_GETFILE)getAddress(_T("GetFile")))!=NULL){
		result=static_cast<SPI_ERROR>(GetFile(str::utf162sjis(file_path).c_str(),buffer_length,dest,flag,callback,data));
	}
	return result;
}

//Plug-in設定ダイアログの表示 
int SpiBase::ConfigurationDlg(HWND wnd_handle,int fnc){
	typedef int(WINAPI*SPI_CONFIGURATIONDLG)(HWND,int);
	SPI_CONFIGURATIONDLG ConfigurationDlg;
	int result=0;

	if((ConfigurationDlg=(SPI_CONFIGURATIONDLG)getAddress(_T("ConfigurationDlg")))!=NULL){
		result=ConfigurationDlg(wnd_handle,fnc);
	}

	return result;
}
