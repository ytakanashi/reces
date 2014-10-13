//CfgFile.cpp
//設定ファイル読み書き

#include"../StdAfx.h"
#include"sslib.h"

namespace sslib{

namespace{
	void writeBom(const TCHAR* file_path){
#ifdef UNICODE
		//cfgファイルが存在しない場合新規作成し、UTF16LEBOMを書き込む
		File cfg_file(file_path,CREATE_NEW,GENERIC_READ|GENERIC_WRITE,0,File::UTF16LE);

		if(cfg_file.isOpened()){
			cfg_file.writeBom();
		}
#endif
	}
}

CfgFile::CfgFile():m_empty(true),m_cfg_path(){
	std::vector<TCHAR> full_path(MAX_PATH);
	tstring cfg_path;

	if(::GetModuleFileName(NULL,&full_path[0],full_path.size())){
		cfg_path.assign(path::removeExtension(&full_path[0]))+=_T(".cfg");
		setFileName(cfg_path.c_str());
	}
}

CfgFile::CfgFile(const TCHAR* cfg_path):m_cfg_path(){
	setFileName(cfg_path);
}

CfgFile::~CfgFile(){
}

//cfgファイル名を設定
void CfgFile::setFileName(const TCHAR* cfg_name){
	if(cfg_name!=NULL)m_cfg_path=cfg_name;

	File cfg_file(m_cfg_path.c_str(),OPEN_EXISTING,GENERIC_READ|GENERIC_WRITE,0,File::UTF16LE);

	if(!cfg_file.isOpened()){
		m_empty=true;
	}else{
		m_empty=!cfg_file.getSize();
	}
}

//キーが存在するか
bool CfgFile::keyExists(const TCHAR* section,const TCHAR* key){
	return ::GetPrivateProfileInt(section,key,0,filepath().c_str())==
		   ::GetPrivateProfileInt(section,key,1,filepath().c_str());
}

//GetPrivateProfileString()でデータを取得
bool CfgFile::getStringData(const TCHAR* section,const TCHAR* key,TCHAR* result,int result_buffer,const TCHAR* default_value){
	::GetPrivateProfileString(section,key,default_value,result,result_buffer,filepath().c_str());
	return keyExists(section,key);
}

//GetPrivateProfileString()でデータを取得
bool CfgFile::getStringData(const TCHAR* section,const TCHAR* key,tstring* result,const TCHAR* default_value){
	std::vector<TCHAR> buffer(MAX_PATHW);

	::GetPrivateProfileString(section,key,default_value,&buffer[0],buffer.size(),filepath().c_str());
	result->assign(&buffer[0]);
	return keyExists(section,key);
}

//存在するならGetPrivateProfileString()でデータを取得
bool CfgFile::getStringDataEx(const TCHAR* section,const TCHAR* key,TCHAR* result,int result_buffer,const TCHAR* default_value){
	if(keyExists(section,key)){
		::GetPrivateProfileString(section,key,default_value,result,result_buffer,filepath().c_str());
	}
	return keyExists(section,key);
}

//存在するならGetPrivateProfileString()でデータを取得
bool CfgFile::getStringDataEx(const TCHAR* section,const TCHAR* key,tstring* result,const TCHAR* default_value){
	if(keyExists(section,key)){
		std::vector<TCHAR> buffer(MAX_PATHW);

		::GetPrivateProfileString(section,key,default_value,&buffer[0],buffer.size(),filepath().c_str());
		result->assign(&buffer[0]);
	}
	return keyExists(section,key);
}

//データを設定
bool CfgFile::setData(const TCHAR* section,const TCHAR* key,int value){
	if(m_empty){writeBom(filepath().c_str());m_empty=false;}
	VariableArgument va(_T("%d"),value);
	return setData(section,key,va.get());
}

//データ(文字列)を設定
bool CfgFile::setData(const TCHAR* section,const TCHAR* key,const TCHAR* value){
	if(m_empty){writeBom(filepath().c_str());m_empty=false;}
	return ::WritePrivateProfileString(section,key,value,filepath().c_str())!=0;
}

//namespace sslib
}
