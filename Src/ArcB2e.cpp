//ArcB2e.cpp
//B2e.dll操作クラス

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r23 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#include"StdAfx.h"
#include"ArcB2e.h"
#include"ArcCfg.h"

using namespace sslib;


ArcB2e::ArcB2e():
	ArcDll(_T("b2e32"),
			_T("B2E"),
			_T("*"),
			_T("\\")){
		COMPRESSION_METHOD method[]={
			{NULL,NULL,NULL,NULL,0,-1,-1,-1}
		};
		m_compression_methods.assign(method,method+ARRAY_SIZEOF(method));
}

ArcB2e::ARC_RESULT ArcB2e::compress(const TCHAR* arc_path,std::list<tstring>* file_list,tstring* log_msg){
	return ARC_FAILURE;
}

ArcB2e::ARC_RESULT ArcB2e::extract(const TCHAR* arc_path,const TCHAR* output_dir,tstring* log_msg){
	return ARC_FAILURE;
}

ArcB2e::ARC_RESULT ArcB2e::list(const TCHAR* arc_path){
	return ARC_FAILURE;
}

//圧縮対象ファイルのパスを整形してファイルに書き出す
DWORD ArcB2e::writeFormatedPath(const File& list_file,const TCHAR* base_dir,const TCHAR* file_path){
	return -1;
}

//圧縮対象ファイルリストを整形してファイルに書き出す
bool ArcB2e::writeFormatedList(const File& list_file,const tstring& full_path){
	return false;
}

//b2eスクリプトのあるディレクトリを指定
bool ArcB2e::setScriptDirectory(const TCHAR* dir_path){
	typedef BOOL(WINAPI*SETSCRIPTDIRECTORY_PTR)(const char*);
	SETSCRIPTDIRECTORY_PTR p_setScriptDirectory;
	bool result=false;

	if((p_setScriptDirectory=(SETSCRIPTDIRECTORY_PTR)getAddress(_T("SetScriptDirectory")))!=NULL){
		result=p_setScriptDirectory((isUnicodeMode()?str::utf162utf8(dir_path):str::utf162sjis(dir_path)).c_str())!=0;
	}
	return result;
}

//b2eスクリプトの数を取得
int ArcB2e::getScriptCount(){
	typedef int(WINAPI*SCRIPTGETCOUNT_PTR)();
	SCRIPTGETCOUNT_PTR p_ScriptGetCount;
	int result=-1;

	if((p_ScriptGetCount=(SCRIPTGETCOUNT_PTR)getAddress(_T("ScriptGetCount")))!=NULL){
		result=p_ScriptGetCount();
	}
	return result;
}

//b2eスクリプト名を取得
bool ArcB2e::getScriptName(const UINT index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETNAME_PTR)(const UINT,char*,DWORD);
	SCRIPTGETNAME_PTR p_ScriptGetName;
	bool result=false;

	if((p_ScriptGetName=(SCRIPTGETNAME_PTR)getAddress(_T("ScriptGetName")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetName(index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//b2eスクリプトの能力を列挙
ArcB2e::B2E_ABILITY ArcB2e::getScriptAbility(const UINT index){
	typedef int(WINAPI*SCRIPTGETABILITY_PTR)(const UINT,DWORD*);
	SCRIPTGETABILITY_PTR p_ScriptGetAbility;
	B2E_ABILITY result=B2E_ABILITY_NONE;
	DWORD ability=0;

	if((p_ScriptGetAbility=(SCRIPTGETABILITY_PTR)getAddress(_T("ScriptGetAbility")))!=NULL){
		if(p_ScriptGetAbility(index,&ability)){
			result=static_cast<B2E_ABILITY>(ability);
		}
	}
	return result;
}

//解凍できる拡張子('.'付き)の_nExtIndex番目のものを取得
bool ArcB2e::getExtractExtensions(const UINT index,const UINT ext_index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETEXTRACTEXTENSIONS_PTR)(const UINT,const UINT,char*,DWORD);
	SCRIPTGETEXTRACTEXTENSIONS_PTR p_ScriptGetExtractExtensions;
	bool result=false;

	if((p_ScriptGetExtractExtensions=(SCRIPTGETEXTRACTEXTENSIONS_PTR)getAddress(_T("ScriptGetExtractExtensions")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetExtractExtensions(index,ext_index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//b2eの圧縮対応形式名を取得(拡張子名でないことに注意)
bool ArcB2e::getCompressType(const UINT index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETCOMPRESSTYPE_PTR)(const UINT,char*,DWORD);
	SCRIPTGETCOMPRESSTYPE_PTR p_ScriptGetCompressType;
	bool result=false;

	if((p_ScriptGetCompressType=(SCRIPTGETCOMPRESSTYPE_PTR)getAddress(_T("ScriptGetCompressType")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetCompressType(index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//圧縮用b2eの対応メソッドを取得
bool ArcB2e::getCompressMethod(const UINT index,const UINT mhd_index,tstring* result_buffer,DWORD buffer_size){
	typedef bool(WINAPI*SCRIPTGETCOMPRESSMETHOD_PTR)(const UINT,const UINT,char*,DWORD);
	SCRIPTGETCOMPRESSMETHOD_PTR p_ScriptGetCompressMethod;
	bool result=false;

	if((p_ScriptGetCompressMethod=(SCRIPTGETCOMPRESSMETHOD_PTR)getAddress(_T("ScriptGetCompressMethod")))!=NULL){
		std::vector<char> buffer_multibyte(buffer_size);

		result=p_ScriptGetCompressMethod(index,mhd_index,&buffer_multibyte[0],buffer_size);
		if(isUnicodeMode()){
			str::utf82utf16(result_buffer,&buffer_multibyte[0]);
		}else{
			str::sjis2utf16(result_buffer,&buffer_multibyte[0]);
		}
	}
	return result;
}

//圧縮用B2Eのデフォルトメソッド番号を取得
int ArcB2e::getDefaultCompressMethod(const UINT index){
	typedef int(WINAPI*SCRIPTGETDEFAULTCOMPRESSMETHOD_PTR)();
	SCRIPTGETDEFAULTCOMPRESSMETHOD_PTR p_ScriptGetDefaultCompressMethod;
	int result=-1;

	if((p_ScriptGetDefaultCompressMethod=(SCRIPTGETDEFAULTCOMPRESSMETHOD_PTR)getAddress(_T("ScriptGetDefaultCompressMethod")))!=NULL){
		result=p_ScriptGetDefaultCompressMethod();
	}
	return result;
}

//指定したアーカイブファイルの解凍に使うB2Eのインデックスを取得
UINT ArcB2e::getExtractorIndex(const TCHAR* file_path){
	typedef UINT(WINAPI*SCRIPTGETEXTRACTORINDEX_PTR)(const char*);
	SCRIPTGETEXTRACTORINDEX_PTR p_ScriptGetExtractorIndex;
	UINT result=-1;

	if((p_ScriptGetExtractorIndex=(SCRIPTGETEXTRACTORINDEX_PTR)getAddress(_T("ScriptGetExtractorIndex")))!=NULL){
		result=p_ScriptGetExtractorIndex((isUnicodeMode()?str::utf162utf8(file_path):str::utf162sjis(file_path)).c_str())!=0;
	}
	return result;
}
