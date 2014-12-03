//WcxBase.cpp
//wcx操作クラス
//一部の関数のみに対応

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r23 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`

#include"StdAfx.h"
#include"WcxBase.h"

using namespace sslib;


HANDLE WcxBase::openArchiveA(tOpenArchiveData* arc_data){
	typedef	HANDLE(WINAPI*WCX_OPENARCHIVE)(tOpenArchiveData*);
	WCX_OPENARCHIVE OpenArchive;

	if((OpenArchive=(WCX_OPENARCHIVE)getAddress(_T("OpenArchive")))!=NULL){
		m_arc_handle=OpenArchive(arc_data);
	}
	return m_arc_handle;
}

HANDLE WcxBase::openArchiveW(tOpenArchiveDataW* arc_data){
	typedef	HANDLE(WINAPI*WCX_OPENARCHIVEW)(tOpenArchiveDataW*);
	WCX_OPENARCHIVEW OpenArchiveW;

	if((OpenArchiveW=(WCX_OPENARCHIVEW)getAddress(_T("OpenArchiveW")))!=NULL){
		m_arc_handle=OpenArchiveW(arc_data);
	}
	return m_arc_handle;
}

HANDLE WcxBase::openArchive(){
	if(m_open_arc_data==NULL)return NULL;
	return (m_open_arc_data->is_unicode_mode)?
		openArchiveW(&m_open_arc_data->open_dataW):
		openArchiveA(&m_open_arc_data->open_dataA);
}

int WcxBase::readHeader(tHeaderData* header_data){
	typedef int(WINAPI*WCX_READHEADER)(HANDLE,tHeaderData*);
	WCX_READHEADER ReadHeader;
	int result=E_NOT_SUPPORTED;

	if((ReadHeader=(WCX_READHEADER)getAddress(_T("ReadHeader")))!=NULL){
		result=ReadHeader(m_arc_handle,header_data);
	}
	return result;
}

int WcxBase::readHeaderEx(tHeaderDataEx* header_data){
	typedef int(WINAPI*WCX_READHEADEREX)(HANDLE,tHeaderDataEx*);
	WCX_READHEADEREX ReadHeaderEx;
	int result=E_NOT_SUPPORTED;

	if((ReadHeaderEx=(WCX_READHEADEREX)getAddress(_T("ReadHeaderEx")))!=NULL){
		result=ReadHeaderEx(m_arc_handle,header_data);
	}
	return result;
}

int WcxBase::readHeaderExW(tHeaderDataExW* header_data){
	typedef int(WINAPI*WCX_READHEADEREXW)(HANDLE,tHeaderDataExW*);
	WCX_READHEADEREXW ReadHeaderExW;
	int result=E_NOT_SUPPORTED;

	if((ReadHeaderExW=(WCX_READHEADEREXW)getAddress(_T("ReadHeaderExW")))!=NULL){
		result=ReadHeaderExW(m_arc_handle,header_data);
	}
	return result;
}

int WcxBase::readHeader(){
	if(m_header_data==NULL)return E_EREAD;
	return (m_header_data->hd_mode==HEADER_DATA::HDA)?
		readHeader(&m_header_data->header_dataA):
		(m_header_data->hd_mode==HEADER_DATA::HDEXA)?
		readHeaderEx(&m_header_data->header_data_exA):
		readHeaderExW(&m_header_data->header_data_exW);
}

int WcxBase::processFileA(int operation,char*dest_path,char*dest_name){
	typedef int(WINAPI*WCX_PROCESSFILE)(HANDLE,int,char*,char*);
	WCX_PROCESSFILE ProcessFile;
	int result=E_NOT_SUPPORTED;

	if((ProcessFile=(WCX_PROCESSFILE)getAddress(_T("ProcessFile")))!=NULL){
		result=ProcessFile(m_arc_handle,operation,dest_path,dest_name);
	}
	return result;
}

int WcxBase::processFileW(int operation,TCHAR*dest_path,TCHAR*dest_name){
	typedef int(WINAPI*WCX_PROCESSFILEW)(HANDLE,int,wchar_t*,wchar_t*);
	WCX_PROCESSFILEW ProcessFileW;
	int result=E_NOT_SUPPORTED;

	if((ProcessFileW=(WCX_PROCESSFILEW)getAddress(_T("ProcessFileW")))!=NULL){
		result=ProcessFileW(m_arc_handle,operation,dest_path,dest_name);
	}
	return result;
}

int WcxBase::processFile(int operation,const TCHAR*dest_path,const TCHAR*dest_name){
	if(m_open_arc_data->is_unicode_mode){
		wchar_t dest_path_[1024]={};
		wchar_t dest_name_[1024]={};
		if(dest_path)lstrcpyn(dest_path_,dest_path,1024);
		if(dest_name)lstrcpyn(dest_name_,dest_name,1024);
		return processFileW(operation,(dest_path)?dest_path_:NULL,(dest_name)?dest_name_:NULL);
	}else{
		char dest_path_[1024]={};
		char dest_name_[1024]={};
		if(dest_path)lstrcpynA(dest_path_,str::utf162sjis(dest_path).c_str(),1024);
		if(dest_name)lstrcpynA(dest_name_,str::utf162sjis(dest_name).c_str(),1024);
		return processFileA(operation,(dest_path)?dest_path_:NULL,(dest_name)?dest_name_:NULL);
	}
}

bool WcxBase::closeArchive(){
	typedef	int(WINAPI*WCX_CLOSEARCHIVE)(HANDLE);
	WCX_CLOSEARCHIVE CloseArchive;
	bool result=false;

	if(!isOpened())return false;

	if((CloseArchive=(WCX_CLOSEARCHIVE)getAddress(_T("CloseArchive")))!=NULL){
		result=CloseArchive(m_arc_handle)==0;
	}
	m_arc_handle=NULL;
	return result;
}

bool WcxBase::configurePacker(HWND wnd_handle,HINSTANCE inst){
	typedef void(WINAPI*WCX_CONFIGUREPACKER)(HWND,HINSTANCE);
	WCX_CONFIGUREPACKER ConfigurePacker;

	if((ConfigurePacker=(WCX_CONFIGUREPACKER)getAddress(_T("ConfigurePacker")))!=NULL){
		ConfigurePacker(wnd_handle,inst);
		return true;
	}
	return false;
}


void WcxBase::setProcessDataProc(tProcessDataProc process_data_proc){
	typedef	void(WINAPI*WCX_SETPROCESSDATAPROC)(HANDLE,tProcessDataProc);
	WCX_SETPROCESSDATAPROC SetProcessDataProc;

	if((SetProcessDataProc=(WCX_SETPROCESSDATAPROC)getAddress(_T("SetProcessDataProc")))!=NULL){
		SetProcessDataProc(m_arc_handle,process_data_proc);
	}
	return;
}

void WcxBase::setProcessDataProcW(tProcessDataProcW process_data_proc){
	typedef	void(WINAPI*WCX_SETPROCESSDATAPROCW)(HANDLE,tProcessDataProcW);
	WCX_SETPROCESSDATAPROCW SetProcessDataProcW;

	if((SetProcessDataProcW=(WCX_SETPROCESSDATAPROCW)getAddress(_T("SetProcessDataProcW")))!=NULL){
		SetProcessDataProcW(m_arc_handle,process_data_proc);
	}
	return;
}

bool WcxBase::isUnicodeMode(){
	return getAddress(_T("OpenArchiveW"))!=NULL;
	return false;
}

int WcxBase::getHeaderDataMode(){
	if(getAddress(_T("ReadHeaderExW"))!=NULL){
		return HEADER_DATA::HDEXW;
	}else if(getAddress(_T("ReadHeaderExA"))!=NULL){
		return HEADER_DATA::HDEXA;
	}else{
		return HEADER_DATA::HDA;
	}
}
