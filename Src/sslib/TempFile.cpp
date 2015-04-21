//TempFile.cpp
//一時ファイル

#include"../StdAfx.h"
#include"sslib.h"
#include<ctype.h>


namespace sslib{
namespace tempfile{

#ifndef DISABLE_FILEO_PERATION

//一意の名前を持つディレクトリ名を取得
tstring generateDirName(const TCHAR* prefix,const TCHAR* base_dir){
	std::vector<TCHAR> temp_dir(MAX_PATHW);

	if(base_dir!=NULL){
		if(!path::fileExists(base_dir)){
			if(!fileoperation::createDirectory(base_dir))return _T("");
		}
	}

	::GetTempFileName(path::getTempDirPath().c_str(),prefix,0,&temp_dir[0]);
	::DeleteFile(&temp_dir[0]);

	return (base_dir!=NULL)?path::addTailSlash(base_dir)+=path::getFileName(&temp_dir[0]):&temp_dir[0];
}

//一意の名前を持つディレクトリを作成
tstring createDir(const TCHAR* prefix,const TCHAR* base_dir){
	tstring dir_name(generateDirName(prefix,base_dir));

	return (!dir_name.empty()&&fileoperation::createDirectory(dir_name.c_str()))?dir_name:_T("");
}

//一意の名前を持つファイル名を取得
tstring generateName(const TCHAR* prefix,const TCHAR* base_dir){
	tstring file_name(create(prefix,base_dir));

	if(!file_name.empty()){
		::DeleteFile(file_name.c_str());
	}
	return file_name;
}

//一意の名前を持つファイルを作成
tstring create(const TCHAR* prefix,const TCHAR* base_dir){
	std::vector<TCHAR> temp_dir(MAX_PATH);

	if(base_dir!=NULL){
		lstrcpyn(&temp_dir[0],base_dir,temp_dir.size());
		if(!path::fileExists(base_dir)){
			if(!fileoperation::createDirectory(base_dir))return _T("");
		}
	}else{
		lstrcpyn(&temp_dir[0],path::getTempDirPath().c_str(),temp_dir.size());
	}

	return (::GetTempFileName(&temp_dir[0],prefix,0,&temp_dir[0])!=0)?&temp_dir[0]:_T("");
}

#endif

//namespace tempfile
}
//namespace sslib
}
